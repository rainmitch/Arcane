#ifndef ARCANE_RESOURCEMANAGER_H
#define ARCANE_RESOURCEMANAGER_H

#include <iostream>

#include <arcane/image.h>
#include <map>
#include "mesh.h"
#include <arcane/objLoader.h>

namespace arcane
{
	template<typename T>
	class Resource
	{
	public:
		Resource (const T &obj)
		{
			this->obj = new T (obj);
		}

		Resource (T *obj)
		{
			this->obj = obj;
		}

		Resource &operator= (const T &obj)
		{
			if (obj != nullptr)
				delete obj;
			this->obj = new T (obj);

			return *this;
		}

		operator T *()
		{
			return obj;
		}

		T *operator-> ()
		{
			return obj;
		}

		~Resource ()
		{
			delete obj;
		}

		T *obj = nullptr;
	};
	namespace ResourceManager
	{

		template<typename T>
		Resource<T> load (const std::string &path)
		{
			return Resource<T> (T (path));
		}

		template<>
		Resource<Image> load (const std::string &path)
		{
			Image img = Image ();

			std::string extension = path.substr (path.size () - 3, 3);

			if (extension == "png")
			{
				img = loadPNGTexture (path);
			}

			return Resource<Image> (img);
		}

		template<>
		Resource<Mesh> load (const std::string &path)
		{
			Mesh mesh = Mesh ();

			std::string extension = path.substr (path.size () - 3, 3);
			if (extension == "obj")
				mesh = loadOBJ (path);

			return Resource<Mesh> (mesh);
		}

		void terminate ();
	}
}

#endif //ARCANE_RESOURCEMANAGER_H
