
#include "renderer.h"

#include <algorithm>

#include <arcane/mesh.h>
#include <arcane/WindowRendering/window.h>

#include "internal.h"
#include "batchingObject.h"

#include <GL/glew.h>
#include <map>


const std::string finalVert = R"(
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

const std::string finalFrag = R"(
#version 430 core

in vec2 textureCoord;

out vec3 pixel;

layout(location=0) uniform sampler2D pbrLayer;
layout(location=1) uniform sampler2D uiLayer;
layout(location=2) uniform sampler2D albedo;


void main ()
{
	vec3 pbr = texture2D (pbrLayer, textureCoord).rgb;

	vec4 uiRaw = texture2D (uiLayer, textureCoord);
	vec3 graphics = pbr;
	vec3 total = (graphics * (1.0f-uiRaw.a)) + (uiRaw.rgb * uiRaw.a);
	pixel = min (total, vec3 (1, 1, 1));

	//pixel = texture2D (albedo, textureCoord).rgb;
}
)";

const std::string skyboxVert = R"(
#version 430 core

layout(location = 0) in vec3 vPosition;

out vec3 textureCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main ()
{
	textureCoord = vPosition;
    vec4 pos = projectionMatrix * viewMatrix * vec4(vPosition, 1.0);
    gl_Position = pos.xyww;
}
)";

const std::string skyboxFrag = R"(
#version 430 core

in vec3 textureCoord;

layout(location = 0) out vec4 albedoPlusSpec;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec2 materials;
layout(location = 4) out vec2 metalnessRoughness;
out vec4 pixel;

uniform samplerCube skybox;

void main ()
{
	albedoPlusSpec = texture (skybox, textureCoord);
	materials.r = 0;

	//pixel = vec4 (1);
}
)";

const vec3 screenVerts[6] = {vec3 (-1, -1, 0), vec3 (1, -1, 0), vec3 (1, 1, 0),
							 vec3 (-1, -1, 0), vec3 (1, 1, 0), vec3 (-1, 1, 0)};
const vec2 screenUV[6] = {
		vec2 (0, 0), vec2 (1, 0), vec2 (1, 1),
		vec2 (0, 0), vec2 (1, 1), vec2 (0, 1)
};

const float skyboxVertices[] = {
		// positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
};
namespace arcane
{
	namespace OpenGL43
	{
		GLuint skymapID;
		GLuint skymapvbo;
		GLuint skymapVao;

		std::map<std::string, BatchingObject> batchedObjects;
		GLuint screenVbo;
		GLuint screenUVBuffer;
		GLuint screenVao;

		GLuint pbrFBO;
		GLuint pbrAlbedoPlusSpec;
		GLuint pbrPosition;
		GLuint pbrNormal;
		GLuint pbrMaterials;
		GLuint pbrMetalnessRoughness;
		GLuint pbrRbo;
		GLuint pbrLayer;

		GLuint mixdownFBO;

		GLuint uiLayer;

		bool *renderableObjs = nullptr;

		void GLAPIENTRY
		MessageCallback( GLenum source,
						 GLenum type,
						 GLuint id,
						 GLenum severity,
						 GLsizei length,
						 const GLchar* message,
						 const void* userParam )
		{
			if (severity != GL_DEBUG_SEVERITY_MEDIUM && severity != GL_DEBUG_SEVERITY_HIGH)
				return;
			fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
					 ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
					 type, severity, message );
		}

		void init ()
		{
			glewExperimental = true;

			if (glewInit () != GLEW_OK)
			{
				throw std::runtime_error ("Could not initialize GLEW!");
			}

			glEnable (GL_DEBUG_OUTPUT);
			glDebugMessageCallback (MessageCallback, 0);

			auto s = Window::size ();
			glGenTextures (1, &uiLayer);
			glBindTexture (GL_TEXTURE_2D, uiLayer);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, s.first, s.second, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture (GL_TEXTURE_2D, 0);


			glGenFramebuffers (1, &pbrFBO);
			glBindFramebuffer (GL_FRAMEBUFFER, pbrFBO);

			// set up pbr framebuffer's depth texture
			glGenRenderbuffers (1, &pbrRbo);
			glBindRenderbuffer (GL_RENDERBUFFER, pbrRbo);
			glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, s.first, s.second);
			glBindRenderbuffer (GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pbrRbo);

			if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error ("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");


			glGenTextures (1, &pbrAlbedoPlusSpec);
			glBindTexture (GL_TEXTURE_2D, pbrAlbedoPlusSpec);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA16F, s.first, s.second, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pbrAlbedoPlusSpec, 0);

			glGenTextures (1, &pbrPosition);
			glBindTexture (GL_TEXTURE_2D, pbrPosition);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, s.first, s.second, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, pbrPosition, 0);

			glGenTextures (1, &pbrNormal);
			glBindTexture (GL_TEXTURE_2D, pbrNormal);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, s.first, s.second, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, pbrNormal, 0);

			glGenTextures (1, &pbrMaterials);
			glBindTexture (GL_TEXTURE_2D, pbrMaterials);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_R16, s.first, s.second, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, pbrMaterials, 0);

			glGenTextures (1, &pbrMetalnessRoughness);
			glBindTexture (GL_TEXTURE_2D, pbrMetalnessRoughness);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, s.first, s.second, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, pbrMetalnessRoughness, 0);

			unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
								 GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
			glDrawBuffers (5, attachments);

			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glGenFramebuffers (1, &mixdownFBO);
			glBindFramebuffer (GL_FRAMEBUFFER, mixdownFBO);

			glGenTextures (1, &pbrLayer);
			glBindTexture (GL_TEXTURE_2D, pbrLayer);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, s.first, s.second, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture (GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pbrLayer, 0);

			unsigned int attachments2[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers (1, attachments2);
			glBindFramebuffer (GL_FRAMEBUFFER, 0);

			Shader::registerShaders ("final", finalVert, finalFrag);

			glGenVertexArrays (1, &screenVao);
			glBindVertexArray (screenVao);

			glEnableVertexAttribArray (0);
			glGenBuffers (1, &screenVbo);
			glBindBuffer (GL_ARRAY_BUFFER, screenVbo);
			glBufferData (GL_ARRAY_BUFFER, sizeof (screenVerts), screenVerts, GL_STATIC_DRAW);
			glGenBuffers (1, &screenUVBuffer);
			glBindBuffer (GL_ARRAY_BUFFER, screenUVBuffer);
			glBufferData (GL_ARRAY_BUFFER, sizeof (screenUV), screenUV, GL_STATIC_DRAW);
			glBindBuffer (GL_ARRAY_BUFFER, screenVbo);
			glVertexAttribPointer (0, 3, GL_FLOAT, false, 0, nullptr);
			glEnableVertexAttribArray (1);
			glBindBuffer (GL_ARRAY_BUFFER, screenUVBuffer);
			glVertexAttribPointer (1, 2, GL_FLOAT, false, 0, nullptr);

			glGenVertexArrays (1, &skymapVao);
			glBindVertexArray (skymapVao);
			glGenTextures (1, &skymapID);
			glGenBuffers (1, &skymapvbo);
			glBindBuffer (GL_ARRAY_BUFFER, skymapvbo);
			glBufferData (GL_ARRAY_BUFFER, sizeof (skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray (0);
			glVertexAttribPointer (0, 3, GL_FLOAT, false, 0, nullptr);

			Shader::registerShaders ("skybox", skyboxVert, skyboxFrag);

			glBindVertexArray (0);
			glBindTexture (GL_TEXTURE_2D, 0);
		}

		void registerMesh (Mesh *mesh)
		{
			batchedObjects[mesh->getShader ()].push (mesh);
		}

		void deregisterMesh (Mesh *mesh)
		{
			batchedObjects[mesh->getShader ()].remove (mesh);
		}

		void renderFrame (const mat4 &projectionMatrix, const mat4 &viewMatrix)
		{
			glBindFramebuffer (GL_FRAMEBUFFER, pbrFBO);
			//glBindRenderbuffer (GL_RENDERBUFFER, pbrRbo);

			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glClearColor (0, 0, 0, 1);
			glEnable (GL_DEPTH_TEST);
			glDepthFunc (GL_LESS);
			glEnable (GL_CULL_FACE);
			glCullFace (GL_BACK);
			glFrontFace (GL_CCW);



			unsigned int index = 1;
			for (auto &bobj : batchedObjects)
			{
				bobj.second.update ();
				bobj.second.render (index, projectionMatrix, viewMatrix);
				index++;
			}

			{ // start skybox
				glDepthFunc (GL_LEQUAL);
				uint skyboxProgramID = Shader::getShaderID ("skybox");
				glUseProgram (skyboxProgramID);

				glBindVertexArray (skymapVao);

				mat4 modifiedViewMatrix = mat4 (mat3 (viewMatrix));

				glUniformMatrix4fv (glGetUniformLocation (skyboxProgramID, "viewMatrix"), 1, GL_FALSE,
									&modifiedViewMatrix[0][0]);
				glUniformMatrix4fv (glGetUniformLocation (skyboxProgramID, "projectionMatrix"), 1, GL_FALSE,
									&projectionMatrix[0][0]);

				glActiveTexture (GL_TEXTURE0);
				glBindTexture (GL_TEXTURE_CUBE_MAP, skymapID);
				glUniform1i (glGetUniformLocation (skyboxProgramID, "skybox"), 0);

				//glEnableVertexAttribArray (0);
				//glBindBuffer (GL_ARRAY_BUFFER, skymapvbo);
				//glVertexAttribPointer (0, 3, GL_FLOAT, false, 0, nullptr);

				glDrawArrays (GL_TRIANGLES, 0, 36);
			}// end skybox

			glBindFramebuffer (GL_FRAMEBUFFER, mixdownFBO);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glEnable (GL_DEPTH_TEST);
			glDepthFunc (GL_LESS);
			glClearColor (0, 0, 0, 1);


			{ // start pbr
				unsigned int programID = Shader::getShaderID ("pbr");
				glUseProgram (programID);

				glActiveTexture (GL_TEXTURE0);
				glBindTexture (GL_TEXTURE_2D, pbrAlbedoPlusSpec);
				glActiveTexture (GL_TEXTURE1);
				glBindTexture (GL_TEXTURE_2D, pbrPosition);
				glActiveTexture (GL_TEXTURE2);
				glBindTexture (GL_TEXTURE_2D, pbrNormal);
				glActiveTexture (GL_TEXTURE3);
				glBindTexture (GL_TEXTURE_2D, pbrMaterials);
				glActiveTexture (GL_TEXTURE4);
				glBindTexture (GL_TEXTURE_2D, pbrMetalnessRoughness);

				glBindVertexArray (screenVao);
				glUniform1i (0, 0);
				glUniform1i (1, 1);
				glUniform1i (2, 2);
				glUniform1i (3, 3);
				glUniform1i (4, 4);

				auto lights = Window::getScene ()->getLights ();
				vec3 cameraPos = Window::getScene ()->getCamera ()->transform.position;

				for (size_t i = 0; i < lights.size (); i++)
				{
					if (i >= MAX_LIGHTS)
						break;

					glUniform3f (
							glGetUniformLocation (programID, ("lights[" + std::to_string (i) + "].position").c_str ()),
							lights[i]->position.x, lights[i]->position.y, lights[i]->position.z);
					glUniform3f (
							glGetUniformLocation (programID, ("lights[" + std::to_string (i) + "].color").c_str ()),
							lights[i]->color.r, lights[i]->color.g, lights[i]->color.b);
					glUniform1f (
							glGetUniformLocation (programID, ("lights[" + std::to_string (i) + "].intensity").c_str ()),
							lights[i]->intensity);
				}
				glUniform1i (glGetUniformLocation (programID, "numberOfLights"),
							 (lights.size () < MAX_LIGHTS ? lights.size () : MAX_LIGHTS));


				glUniform3f (glGetUniformLocation (programID, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
				glUniform1ui (glGetUniformLocation (programID, "materialID"), 1);

				glDrawArrays (GL_TRIANGLES, 0, 6);
			} // finish pbr

			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glClearColor (0, 0, 0, 1);
			glEnable (GL_DEPTH_TEST);
			glDepthFunc (GL_LESS);

			unsigned int programID2 = Shader::getShaderID ("final");
			glUseProgram (programID2);

			glActiveTexture (GL_TEXTURE0);
			glBindTexture (GL_TEXTURE_2D, pbrLayer);
			glActiveTexture (GL_TEXTURE1);
			glBindTexture (GL_TEXTURE_2D, uiLayer);
			glActiveTexture (GL_TEXTURE2);
			glBindTexture (GL_TEXTURE_2D, pbrAlbedoPlusSpec);

			glUniform1i (0, 0);
			glUniform1i (1, 1);
			glUniform1i (2, 2);


			glDrawArrays (GL_TRIANGLES, 0, 6);
		}

		void renderUI (const ArcBuffer &buffer)
		{
			// todo: explore using a pixel buffer object instead
			ArcSize size = buffer.size ();
			glBindTexture (GL_TEXTURE_2D, uiLayer);

			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, size.width (), size.height (), 0, GL_RGBA, GL_UNSIGNED_BYTE,
						  buffer.data ());
		}

		void resizeFrame (const unsigned int &width, const unsigned int &height)
		{
			glViewport (0, 0, width, height);
			glBindTexture (GL_TEXTURE_2D, pbrAlbedoPlusSpec);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glBindTexture (GL_TEXTURE_2D, pbrPosition);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glBindTexture (GL_TEXTURE_2D, pbrNormal);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glBindTexture (GL_TEXTURE_2D, pbrMaterials);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glBindTexture (GL_TEXTURE_2D, pbrMetalnessRoughness);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);
			glBindTexture (GL_TEXTURE_2D, pbrLayer);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);

			glBindRenderbuffer (GL_RENDERBUFFER, pbrRbo);
			glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		}

		int maxTextureSize ()
		{
			int maxSize;
			glGetIntegerv (GL_MAX_TEXTURE_SIZE, &maxSize);

			return maxSize;
		}

		void markRenderableObjects (bool *renderableObjects, const size_t &count)
		{
			renderableObjs = renderableObjects;
		}

		void setSkymap (const std::vector<Image *> &images)
		{
			glBindTexture (GL_TEXTURE_CUBE_MAP, skymapID);

			for (int i = 0; i < images.size (); i++)
			{
				glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, images[i]->width, images[i]->height, 0,
				  GL_RGBA, GL_UNSIGNED_BYTE, images[i]->data.data());
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	}
}