#include <iostream>
#include <chrono>

#include <arcane/internal.h>
#include <arcane/jobSystem/JobSystem.h>
#include <arcane/scene.h>
#include <arcane/WindowRendering/window.h>
#include <arcane/WindowRendering/shader.h>
#include <arcane/object.h>
#include <arcane/objLoader.h>
#include <arcane/image.h>
#include <arcane/resourceManager.h>
#include <fstream>
#include <arcane/UI/arcButton.h>

#include <arcane/script.h>
#include <arcane/time.h>
#include <arcane/WindowRendering/input.h>
#include <arcane/WindowRendering/computeShader.h>

#include <arcane/WindowRendering/renderer.h>

using namespace arcane;

const std::string vertexShader = R"(
#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 normal;
layout (location = 2 ) in uint drawid;
layout(location = 3) in vec2 vUV;

layout (location = 4) in mat4 modelMatrix;

out uint drawID;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fNormal;
out vec3 worldPos;
out vec2 UV;

void main ()
{
	drawID = drawid;
	worldPos = vec3 (modelMatrix * vec4 (vPosition, 1.0));
	fNormal = (mat3 (modelMatrix) * normal).xyz;
	UV = vUV;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4 (vPosition, 1.0);
}
)";

const std::string fragmentShader = R"(
#version 430 core

precision highp float;

in uint drawID;

layout(location = 0) out vec4 albedoPlusSpec;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec2 materials;
layout(location = 4) out vec2 metalnessRoughness;

in vec3 fNormal;
in vec3 worldPos;
in vec2 UV;

uniform uint materialID;

uniform sampler2D albedoMap;
uniform sampler2D metalnessMap;
uniform sampler2D roughnessMap;

void main ()
{
	vec3 albedo = texture (albedoMap, UV).rgb;
	albedoPlusSpec = vec4 (albedo, 1.0);
	normal = fNormal;
	position = worldPos;
	materials.r = materialID;
	metalnessRoughness.r = texture (metalnessMap, UV).r;
	metalnessRoughness.g = texture (roughnessMap, UV).r;
}
)";

const std::string pbrVertex = R"(
#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 texCoord;

out vec2 textureCoord;

void main ()
{
	textureCoord = texCoord;
	gl_Position = vec4 (vPosition, 1.0);
}
)";

const std::string pbrFragment = R"(
#version 430 core

#define MAX_LIGHTS 64

const float PI = 3.1415926f;
const float EPSILON = 0.00001;
const vec3 Fdialectric = vec3 (0.04);

in vec2 textureCoord;
out vec3 pixel;

layout(location=0) uniform sampler2D albedoPlusSpec;
layout(location=1) uniform sampler2D position;
layout(location=2) uniform sampler2D normal;
layout(location=3) uniform usampler2D materials;
layout(location=4) uniform sampler2D metalnessRoughness;


struct Light
{
	vec3 position;
	vec3 color;
	float intensity;
};

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

uniform vec3 cameraPos;
uniform Light[MAX_LIGHTS] lights;
uniform int numberOfLights;
uniform samplerCube skymap;

uniform uint materialID;


void main ()
{
	uint materialIDInstance = texture (materials, textureCoord).r;
	vec3 albedo = texture (albedoPlusSpec, textureCoord).rgb;

	if (materialIDInstance == 0)
	{
		pixel = albedo;
		return;
	}

	vec3 norm = texture (normal, textureCoord).rgb;
	vec3 worldPos = texture (position, textureCoord).xyz;

	vec3 Lo = normalize (cameraPos - worldPos);

	vec3 irradiance = vec3(0.0);

	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, norm);
	up         = cross(norm, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * Lo;

			irradiance += texture(skymap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(nrSamples));


	float metallic = texture (metalnessRoughness, textureCoord).r;
	float roughness = texture (metalnessRoughness, textureCoord).g;

	vec3 N = normalize (norm);

	float ao = 0.0f;


	float cosLo = max (0.0, dot (N, Lo));
	vec3 Lr = 2.0 * cosLo * N - Lo;

	vec3 F0 = mix (Fdialectric, albedo, metallic);

	vec3 directLighting = vec3 (0);
	for (int i = 0; i < numberOfLights; i++)
	{
		vec3 Li = normalize (lights[i].position - worldPos);
		vec3 Lh = normalize (Li + Lo);
		float distance = length (lights[i].position - worldPos);
		float attenuation = lights[i].intensity / (distance*distance);
		vec3 Lradiance = lights[i].color * attenuation;

		float cosLi = max (0.0, dot (N, Li));
		float cosLh = max (0.0, dot (N, Lh));

		vec3 F = fresnelSchlick (F0, max (0.0, dot (Lh, Lo)));
		float D = ndfGGX (cosLh, roughness);
		float G = gaSchlickGGX (cosLi, cosLo, roughness);

		vec3 kd = mix (vec3 (1.0) - F, vec3 (0.0), metallic);

		vec3 diffBRDF = kd * albedo;

		vec3 specBRDF = (F * D * G) / max (EPSILON, 4.0 * cosLi * cosLo);

		directLighting += (diffBRDF + specBRDF) * Lradiance * cosLi;
	}

	vec3 ambient = vec3 (0.04) * albedo * ao;
	vec3 color = irradiance*ambient + directLighting;

	color = color / (color + vec3 (1.0));
	color = pow (color, vec3 (1.0/2.2));


	pixel = color;
	//pixel = (materialIDInstance == 0 ? albedo : color);
}
)";


std::function<void ()> rotateFunc;

Light *l;

class CameraControl : public Script
{
public:
	Camera *camera;
	void Start () override
	{
		camera = Window::getScene ()->getCamera ();
		Input::registerNewInput ("forward", "W");
		Input::registerNewInput ("backward", "S");
		Input::registerNewInput ("left", "a");
		Input::registerNewInput ("right", "d");
		Input::registerNewInput ("leftRotation", "Left");
		Input::registerNewInput ("rightRotation", "Right");
		Input::registerNewInput ("upRotation", "Up");
		Input::registerNewInput ("downRotation", "Down");
	}

	float speed = 2.5;
	float rotationSpeed = 90;

	void Update () override
	{
		if (Input::active ("forward"))
		{
			camera->transform.move (vec3 (0, 0, speed) * Time::delta ());
		}
		if (Input::active ("backward"))
		{
			camera->transform.move (vec3 (0, 0, -speed) * Time::delta ());
		}
		if (Input::active ("left"))
		{
			camera->transform.move (vec3 (speed, 0, 0) * Time::delta ());
		}
		if (Input::active ("right"))
		{
			camera->transform.move (vec3 (-speed, 0, 0) * Time::delta ());
		}

		if (Input::active ("leftRotation"))
		{
			camera->transform.rotation += vec3 (0, -rotationSpeed, 0) * Time::delta ();
		}
		if (Input::active ("rightRotation"))
		{
			camera->transform.rotation += vec3 (0, rotationSpeed, 0) * Time::delta ();
		}
		if (Input::active ("upRotation"))
		{
			camera->transform.rotation += vec3 (rotationSpeed, 0, 0) * Time::delta ();
		}
		if (Input::active ("downRotation"))
		{
			camera->transform.rotation += vec3 (-rotationSpeed, 0, 0) * Time::delta ();
		}

		//l->position = camera->transform.position;
		//std::cout << camera->transform.rotation << std::endl;
	}
};

int main ()
{
	initializeArcane ("New Window", 640, 480);

	try
	{
		Shader::registerShaders ("default", vertexShader, fragmentShader);
		Shader::registerShaders ("pbr", pbrVertex, pbrFragment);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what () << std::endl;

		return 1;
	}


	Scene scene = Scene ();
	Resource<Image> skymapLeft = ResourceManager::load<Image> ("./textures/sky_left.png");
	Resource<Image> skymapFront = ResourceManager::load<Image> ("./textures/sky_front.png");
	Resource<Image> skymapRight = ResourceManager::load<Image> ("./textures/sky_right.png");
	Resource<Image> skymapBack = ResourceManager::load<Image> ("./textures/sky_back.png");
	Resource<Image> skymapTop = ResourceManager::load<Image> ("./textures/sky_top.png");
	Resource<Image> skymapBottom = ResourceManager::load<Image> ("./textures/sky_bottom.png");


	std::vector<Image *> skymap = {skymapRight, skymapLeft, skymapTop, skymapBottom, skymapFront, skymapBack};
	scene.setSkybox (skymap);


	Camera camera = Camera (vec3 (0.5, 0.5, -5.5));
	camera.transform.rotation += vec3 (0, 0, 0);
	Light light = Light (camera.transform.position + vec3 (0.5, 0, 0), vec3 (1, 1, 1), 15);
	Light light2 = Light (camera.transform.position - vec3 (0.5, 0, 0), vec3 (1, 0.1, 0.1), 15);
	Light light3 = Light (camera.transform.position + vec3 (0, 0.5, 0), vec3 (0.1, 0.1, 1), 15);
	Light light4 = Light (camera.transform.position + vec3 (0, 0.5, 0), vec3 (0.1, 1, 0.1), 15);
	//l = &light4;
	//scene.addLight (&light);
	scene.addLight (&light2);
	scene.addLight (&light3);
	scene.addLight (&light4);
	scene.setCamera (&camera);
	Window::setScene (&scene);

	Object *object = new Object ();

	Resource<Mesh> mesh = ResourceManager::load<Mesh> ("./buddha3.obj");

	object->mesh = *mesh;
	object->scale = vec3 (0.15);


	object->transform.position = vec3 (0, 0, 0);

	Material mat = Material (vec3 (1, .3, .2), 0.0, 0.5, 0.1);

	Resource<Image> albedoMapData = ResourceManager::load<Image> ("./textures/metal_albedo.png");
	Resource<Image> metalnessMapData = ResourceManager::load<Image> ("textures/metal_metalness.png");
	Resource<Image> roughnessMapData = ResourceManager::load<Image> ("textures/metal_roughness.png");
	//Image *aoMapData = ResourceManager::load<Image> ("textures/paint_ao.png");

	mat.setAlbedoMap (albedoMapData);
	mat.setMetalnessMap (metalnessMapData);
	mat.setRoughnessMap (roughnessMapData);
	//mat.setAoMap (aoMapData);

	object->mesh.setMaterial (mat);
	object->mesh.calculateVertexNormals();
	//object->mesh.parent (object);

	std::vector<Object *> objs = std::vector<Object *> ();

	size_t xCount = 3;//10;
	size_t yCount = 1;//5;
	size_t zCount = 1;//8;

	for (size_t i = 0; i < xCount; i++)
	{
		for (size_t k = 0; k < yCount; k++)
		{
			for (size_t j = 0; j < zCount; j++)
			{
				objs.emplace_back (new Object (*object));
				objs[objs.size () - 1]->transform.position = vec3 (-1 + i*0.25, 0+k*.5, -2.5 + j*0.25);
				objs[objs.size () - 1]->mesh.parent (objs[objs.size () - 1]);
				//objs[objs.size () - 1]->mesh.setMaterial (mat);
				scene.addObject (objs[objs.size () - 1]);
			}
		}
	}

	Object cameraControlObj = Object (false);
	cameraControlObj.add (new CameraControl ());

	scene.addObject (&cameraControlObj);

	rotateFunc = [&objs, &object, &camera]() {
		for (size_t i = 0; i < objs.size (); i++)
			objs[i]->transform.rotation += vec3 (0, (360 / 3) * Time::delta (), 0);
		//object->transform.rotation.y += (360/3) * (1.0/144);
	};


	ArcObject leftPanel;

	leftPanel.x (new ScreenWidthPercentConstraint (0));
	leftPanel.y (new ScreenHeightPercentConstraint ( 0));
	leftPanel.width (new PixelConstraint (500));
	leftPanel.height (new ScreenHeightPercentConstraint ( 1));
	leftPanel.backgroundColor (ArcPixel ("#343434"));

	ArcButton button = ArcButton ();
	button.x (new CenterParentWidthConstraint ());
	button.y (new PixelConstraint (32));
	button.width (new PixelConstraint (200));
	button.height(new PixelConstraint (75));
	button.backgroundColor (ArcPixel (.3, .3, .3));
	button.borderColor (ArcPixel (0.15, 0.15, 0.15));
	button.borderSize (4);

	button.onClick ([]() {
		std::cout << "Yay!" << std::endl;
	});
	button.onHover ([&button](){
		button.backgroundColor (ArcPixel (0.35, 0.35, 0.35));
	});
	button.onNotHover ([&button] ()
	{
		button.backgroundColor (ArcPixel (0.3, 0.3, 0.3));
	});



	leftPanel.layout ().addObject (&button);


	leftPanel.add (&button);
	Window::layout ().addObject (&leftPanel);

	startArcane ();

	return 0;
}
