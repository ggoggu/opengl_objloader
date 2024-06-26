#include "IObj.h"

IOBJ::IOBJ(const char* filename)
{
	loadFile2(filename);
}

IOBJ::~IOBJ()
{
	for (auto iter = texmapsize.begin(); iter != texmapsize.end(); iter++) {
		std::string mtlname = iter->first;
		std::map<std::string, GLubyte*> tempmtl;
		tempmtl = texmap.at(mtlname);

		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
			std::string name = iter2->first;
			std::vector<int> size = iter2->second;

			GLubyte* texture;
			texture = tempmtl.at(name);

			delete[] texture;

		}
	}

	glDeleteLists(gl_list, 1);


}

bool IOBJ::loadTexture(const char* filename, std::string mtlname, std::string texname)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		printf("textrue file open error \n");
		return false;
	}


	unsigned int k = 0;
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, file); // read the 54-byte header
	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];



	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, file); // read the rest of the data at once

	fclose(file);


	GLubyte* temp_tex = new GLubyte[size];

	for (int i = 0; i < size; i = i + 3) {
		temp_tex[i] = data[i + 2];
		temp_tex[i + 1] = data[i + 1];
		temp_tex[i + 2] = data[i];
	}



	delete data;

	std::vector<int> width_height;
	width_height.push_back(width);
	width_height.push_back(height);

	if (texmap.count(mtlname) != 0) {
		texmap.at(mtlname).insert({ texname,temp_tex });
		texmapsize.at(mtlname).insert({ texname,width_height });
	}
	else {
		std::map<std::string, GLubyte*> now_texmap;
		now_texmap.insert({ texname, temp_tex });
		texmap.insert({ mtlname,now_texmap });

		std::map<std::string, std::vector<int>> temp_sizemap;
		temp_sizemap.insert({ texname, width_height });
		texmapsize.insert({ mtlname, temp_sizemap });
	}





}

bool IOBJ::loadMeterialFile(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("meterial file open error \n");
		return false;
	}


	bool checkm = false;
	char lineHeader[128];
	int res;

	while (1) {


		if (checkm != true) {
			res = fscanf(file, "%s", lineHeader);
		}

		checkm = false;

		if (res == EOF)
			break;
		if (strcmp(lineHeader, "newmtl") == 0)
		{
			char name[128];
			fscanf(file, "%s\n", name);

			std::map<std::string, std::vector<float>> newmtl;

			float num = 0.0, num1 = 0.0, num2 = 0.0, num3 = 0.0;

			while (true) {

				res = fscanf(file, "%s", lineHeader);

				if (res == EOF)
					break;



				if (strcmp(lineHeader, "newmtl") == 0)
				{
					checkm = true;
					break;
				}
				else if (strcmp(lineHeader, "Ns") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f\n", &num);
					vector.push_back(num);
					newmtl.insert({ "Ns",vector });
				}
				else if (strcmp(lineHeader, "Ka") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f %f %f\n", &num1, &num2, &num3);

					vector.push_back(num1);
					vector.push_back(num2);
					vector.push_back(num3);

					newmtl.insert({ "Ka", vector });
				}
				else if (strcmp(lineHeader, "Kd") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f %f %f\n", &num1, &num2, &num3);

					vector.push_back(num1);
					vector.push_back(num2);
					vector.push_back(num3);

					newmtl.insert({ "Kd", vector });
				}
				else if (strcmp(lineHeader, "Ks") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f %f %f\n", &num1, &num2, &num3);

					vector.push_back(num1);
					vector.push_back(num2);
					vector.push_back(num3);

					newmtl.insert({ "Ks", vector });
				}
				else if (strcmp(lineHeader, "Ke") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f %f %f\n", &num1, &num2, &num3);

					vector.push_back(num1);
					vector.push_back(num2);
					vector.push_back(num3);

					newmtl.insert({ "Ke", vector });
				}
				else if (strcmp(lineHeader, "Ni") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f\n", &num);
					vector.push_back(num);
					newmtl.insert({ "Ni",vector });
				}
				else if (strcmp(lineHeader, "d") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f\n", &num);
					vector.push_back(num);
					newmtl.insert({ "d",vector });
				}
				else if (strcmp(lineHeader, "illum") == 0)
				{
					std::vector<float> vector;
					fscanf(file, "%f\n", &num);
					vector.push_back(num);
					newmtl.insert({ "illum",vector });
				}
				else if (strcmp(lineHeader, "map_Kd") == 0)
				{
					char texname[128];

					fscanf(file, "%s\n", texname);
					char newname[128] = "./objfile/";
					strcat(newname, texname);
					loadTexture(newname, name, "map_Kd");

					std::vector<float>vector;
					newmtl.insert({ "map_Kd", vector });
				}
			}
			mtlname.insert({ name, newmtl });
		}


	}
	
	fclose(file);

	return true;
}


bool IOBJ::loadFile2(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("obj file open error \n");
		return false;
	}


	char lineHeader[128];
	int res;

	bool checkv = false;
	int objnumber = 0;

	while (1)
	{

		res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;

		if (strcmp(lineHeader, "mtllib") == 0)
		{

			char meteralname[128];
			fscanf(file, "%s\n", meteralname);
			char newname[128] = "./objfile/";
			strcat(newname, meteralname);
			loadMeterialFile(newname);


		}
		else if (strcmp(lineHeader, "v") == 0) {

			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);

			bool checkm = false;
			int fnumber = 0;

			while (true) {

				if (checkm != true)
					res = fscanf(file, "%s", lineHeader);

				fnumber++;

				checkm = false;


				if (res == EOF)
					break;

				if (strcmp(lineHeader, "v") == 0) {
					vec3 vertex;
					fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
					vertices.push_back(vertex);
				}
				else if (strcmp(lineHeader, "vt") == 0) {
					vec3 uv;
					fscanf(file, "%f %f %f\n", &uv.x, &uv.y, &uv.z);
					uvs.push_back(uv);
				}
				else if (strcmp(lineHeader, "vn") == 0) {
					vec3 normal;
					fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);

					normals.push_back(normal);
				}
				else if (strcmp(lineHeader, "g") == 0)
				{
					char temp[128];
					fscanf(file, "%s\n", temp);

					res = fscanf(file, "%s", lineHeader);


					char temp2[128];
					if (strcmp(lineHeader, "usemtl") == 0)
					{
						
						fscanf(file, "%s\n", temp2);

					}

					std::string tempmtlname(temp2);

					std::vector< std::vector<unsigned short> > tvertexIndices, tuvIndices, tnormalIndices;
					while (true) {
						res = fscanf(file, "%s", lineHeader);
						if (res == EOF)
							break;

						if (strcmp(lineHeader, "f") == 0) {
							if (uvs.size() == 0) {
								int vertexIndex[5], normalIndex[5];
								int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], &vertexIndex[3], &normalIndex[3],
									&vertexIndex[4], &normalIndex[4]);

								if (matches == 8) {

									for (int i = 0; i < 4; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 6)
								{

									for (int i = 0; i < 3; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);

									tuvIndices.push_back(t_uIvector);
									tvertexIndices.push_back(t_vIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 10) {
									for (int i = 0; i < 5; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);
									t_vIvector.push_back(vertexIndex[4]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);
									t_nIvector.push_back(normalIndex[4]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
							}
							else {
								int vertexIndex[5], uvIndex[5], normalIndex[5];
								int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3],
									&vertexIndex[4], &uvIndex[4], &normalIndex[4]);

								if (matches == 12) {

									for (int i = 0; i < 4; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (uvIndex[i] < 0)
											uvIndex[i] = uvs.size() + uvIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);

									std::vector<unsigned short>t_uIvector;
									t_uIvector.push_back(uvIndex[0]);
									t_uIvector.push_back(uvIndex[1]);
									t_uIvector.push_back(uvIndex[2]);
									t_uIvector.push_back(uvIndex[3]);

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 9)
								{

									for (int i = 0; i < 3; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (uvIndex[i] < 0)
											uvIndex[i] = uvs.size() + uvIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);

									std::vector<unsigned short>t_uIvector;
									t_uIvector.push_back(uvIndex[0]);
									t_uIvector.push_back(uvIndex[1]);
									t_uIvector.push_back(uvIndex[2]);

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 15) {
									for (int i = 0; i < 5; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (uvIndex[i] < 0)
											uvIndex[i] = uvs.size() + uvIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);
									t_vIvector.push_back(vertexIndex[4]);

									std::vector<unsigned short>t_uIvector;
									t_uIvector.push_back(uvIndex[0]);
									t_uIvector.push_back(uvIndex[1]);
									t_uIvector.push_back(uvIndex[2]);
									t_uIvector.push_back(uvIndex[3]);
									t_uIvector.push_back(uvIndex[4]);

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);
									t_nIvector.push_back(normalIndex[4]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
							}
						}

						else if (strcmp(lineHeader, "g") == 0) {
							checkm = true;
							break;
						}
						else if (strcmp(lineHeader, "#") == 0) {
							checkm = false;
							break;

						}

						else if (strcmp(lineHeader, "o") == 0) {
							checkm = false;
							break;

						}
						else if (strcmp(lineHeader, "usemtl") == 0) {
							checkm = true;
						}
					}

					std::vector<std::vector<std::vector<unsigned short>>> Index;
					Index.push_back(tvertexIndices);
					Index.push_back(tuvIndices);
					Index.push_back(tnormalIndices);

					face mtlandface;
					mtlandface.Index = Index;
					mtlandface.mtl = tempmtlname;

					facevector.push_back(mtlandface);


					if (checkm != true)
						break;
				}
				else if (strcmp(lineHeader, "usemtl") == 0) {


					char temp2[128];
					fscanf(file, "%s\n", temp2);
					std::string tempmtlname(temp2);

					std::vector< std::vector<unsigned short> > tvertexIndices, tuvIndices, tnormalIndices;
					while (true) {
						res = fscanf(file, "%s", lineHeader);
						if (res == EOF)
							break;

						if (strcmp(lineHeader, "f") == 0) {
							if (uvs.size() == 0) {
								int vertexIndex[10], normalIndex[10];
								int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2], &vertexIndex[3], &normalIndex[3],
									&vertexIndex[4], &normalIndex[4], &vertexIndex[5], &normalIndex[5], &vertexIndex[6], &normalIndex[6], &vertexIndex[7], &normalIndex[7], &vertexIndex[8], &normalIndex[8], &vertexIndex[9], &normalIndex[9]);

								if (matches == 8) {

									for (int i = 0; i < 4; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 6)
								{

									for (int i = 0; i < 3; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 10) {
									for (int i = 0; i < 5; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);
									t_vIvector.push_back(vertexIndex[4]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);
									t_nIvector.push_back(normalIndex[4]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 16) {
									for (int i = 0; i < 8; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);
									t_vIvector.push_back(vertexIndex[4]);
									t_vIvector.push_back(vertexIndex[5]);
									t_vIvector.push_back(vertexIndex[6]);
									t_vIvector.push_back(vertexIndex[7]);

									std::vector<unsigned short>t_uIvector;

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);
									t_nIvector.push_back(normalIndex[4]);
									t_nIvector.push_back(normalIndex[5]);
									t_nIvector.push_back(normalIndex[6]);
									t_nIvector.push_back(normalIndex[7]);



									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
							}
							else {
								int vertexIndex[5], uvIndex[5], normalIndex[5];
								int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3],
									&vertexIndex[4], &uvIndex[4], &normalIndex[4]);

								if (matches == 12) {

									for (int i = 0; i < 4; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (uvIndex[i] < 0)
											uvIndex[i] = uvs.size() + uvIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);

									std::vector<unsigned short>t_uIvector;
									t_uIvector.push_back(uvIndex[0]);
									t_uIvector.push_back(uvIndex[1]);
									t_uIvector.push_back(uvIndex[2]);
									t_uIvector.push_back(uvIndex[3]);

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 9)
								{

									for (int i = 0; i < 3; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (uvIndex[i] < 0)
											uvIndex[i] = uvs.size() + uvIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);

									std::vector<unsigned short>t_uIvector;
									t_uIvector.push_back(uvIndex[0]);
									t_uIvector.push_back(uvIndex[1]);
									t_uIvector.push_back(uvIndex[2]);

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
								else if (matches == 15) {
									for (int i = 0; i < 5; i++) {
										if (vertexIndex[i] < 0)
											vertexIndex[i] = vertices.size() + vertexIndex[i] + 1;
										if (uvIndex[i] < 0)
											uvIndex[i] = uvs.size() + uvIndex[i] + 1;
										if (normalIndex[i] < 0)
											normalIndex[i] = normals.size() + normalIndex[i] + 1;
									}


									std::vector<unsigned short>t_vIvector;
									t_vIvector.push_back(vertexIndex[0]);
									t_vIvector.push_back(vertexIndex[1]);
									t_vIvector.push_back(vertexIndex[2]);
									t_vIvector.push_back(vertexIndex[3]);
									t_vIvector.push_back(vertexIndex[4]);

									std::vector<unsigned short>t_uIvector;
									t_uIvector.push_back(uvIndex[0]);
									t_uIvector.push_back(uvIndex[1]);
									t_uIvector.push_back(uvIndex[2]);
									t_uIvector.push_back(uvIndex[3]);
									t_uIvector.push_back(uvIndex[4]);

									std::vector<unsigned short>t_nIvector;
									t_nIvector.push_back(normalIndex[0]);
									t_nIvector.push_back(normalIndex[1]);
									t_nIvector.push_back(normalIndex[2]);
									t_nIvector.push_back(normalIndex[3]);
									t_nIvector.push_back(normalIndex[4]);

									tvertexIndices.push_back(t_vIvector);
									tuvIndices.push_back(t_uIvector);
									tnormalIndices.push_back(t_nIvector);
								}
							}

						}
						else if (strcmp(lineHeader, "usemtl") == 0) {
							checkm = true;
							break;

						}
						else if (strcmp(lineHeader, "g") == 0) {
							checkm = true;
							break;
						}
						else if (strcmp(lineHeader, "#") == 0) {
							checkm = false;
							break;
						}
						else if (strcmp(lineHeader, "o") == 0) {
							checkm = false;
							break;

						}

					}

					std::vector<std::vector<std::vector<unsigned short>>> Index;
					Index.push_back(tvertexIndices);
					Index.push_back(tuvIndices);
					Index.push_back(tnormalIndices);

					face mtlandface;
					mtlandface.Index = Index;
					mtlandface.mtl = tempmtlname;

					facevector.push_back(mtlandface);

					if (checkm != true)
						break;
				}
			}


		}
	}

	fclose(file);
	return true;
}

/*
void IOBJ::generate()
{
	gl_list = glGenLists(1);
	glNewList(gl_list, GL_COMPILE);
	for (auto iter = facevector.begin(); iter != facevector.end(); iter++) {
		face faceandmtl = *iter;
		std::string t_mtlname = faceandmtl.mtl;
		std::vector<std::vector<std::vector<int>>> faces = faceandmtl.Index;

		std::vector<std::vector<int>> t_vertexIndices, t_uvIndices, t_normalIndices;
		t_vertexIndices = faces[0];
		t_uvIndices = faces[1];
		t_normalIndices = faces[2];

		std::map<std::string, std::vector<float>> usemtl = mtlname[t_mtlname];

		std::vector<float> temp;


		if (usemtl.count("Ka") != 0) {
			temp = usemtl["Ka"];
			GLfloat Ka[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);

		}

		if (usemtl.count("Kd") != 0)
		{
			temp = usemtl["Kd"];
			GLfloat Kd[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);

		}

		if (usemtl.count("Ks") != 0)
		{
			temp = usemtl["Ks"];
			GLfloat Ks[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
		}

		if (usemtl.count("Ke") != 0)
		{
			temp = usemtl["Ke"];
			GLfloat Ke[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_EMISSION, Ke);

		}

		if (usemtl.count("Ns") != 0)
		{

			temp = usemtl["Ns"];
			GLfloat Ns = temp[0];


			glMaterialf(GL_FRONT, GL_SHININESS, Ns);
		}




		if (texmap.count(t_mtlname) != 0) {
			std::map<std::string, std::vector<int>> face_tex_size;
			face_tex_size = texmapsize[t_mtlname];

			std::map<std::string, GLubyte*> face_tex;
			face_tex = texmap.at(t_mtlname);

			if (face_tex.count("map_Kd") != 0) {
				std::vector<int> temp_size_vector = face_tex_size["map_Kd"];
				GLubyte* temp_tex = face_tex["map_Kd"];


				glTexImage2D(GL_TEXTURE_2D, 0, 3, temp_size_vector[0], temp_size_vector[1], 0, GL_RGB, GL_UNSIGNED_BYTE, temp_tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}

			glEnable(GL_TEXTURE_2D);

			register unsigned int indexsize = t_vertexIndices.size();

			for (register unsigned int i = 0; i < indexsize; i++) {
				std::vector<int> vertexindex = t_vertexIndices[i];
				std::vector<int> uvindex = t_uvIndices[i];
				std::vector<int> normalindex = t_normalIndices[i];


				register unsigned int vertexsize = vertexindex.size();
				glBegin(GL_POLYGON);
				for (register unsigned int j = 0; j < vertexsize; j++)
				{

					float x, y, z, nx, ny, nz, ux, uy;

					x = vertices[vertexindex[j] - 1].x, y = vertices[vertexindex[j] - 1].y, z = vertices[vertexindex[j] - 1].z;
					nx = normals[normalindex[j] - 1].x, ny = normals[normalindex[j] - 1].y, nz = normals[normalindex[j] - 1].z;
					ux = uvindex[j] - 1, uy = uvindex[j] - 1;


					float newx = w_matrix[0][0] * x + w_matrix[0][1] * y + w_matrix[0][2] * z + w_matrix[0][3];
					float newy = w_matrix[1][0] * x + w_matrix[1][1] * y + w_matrix[1][2] * z + w_matrix[1][3];
					float newz = w_matrix[2][0] * x + w_matrix[2][1] * y + w_matrix[2][2] * z + w_matrix[2][3];

					float newnx = w_matrix[0][0] * nx + w_matrix[0][1] * ny + w_matrix[0][2] * nz;
					float newny = w_matrix[1][0] * nx + w_matrix[1][1] * ny + w_matrix[1][2] * nz;
					float newnz = w_matrix[2][0] * nx + w_matrix[2][1] * ny + w_matrix[2][2] * nz;



					glNormal3f(newnx, newny, newnz);
					glTexCoord2d(uvs[ux].x, uvs[uy].y);
					glVertex3f(newx, newy, newz);
				}
				glEnd();
			}

			glDisable(GL_TEXTURE_2D);
		}
		else {
			for (register unsigned int i = 0; i < t_vertexIndices.size(); i++) {
				std::vector<int> vertexindex = t_vertexIndices[i];
				std::vector<int> normalindex = t_normalIndices[i];

				glBegin(GL_POLYGON);
				for (register unsigned int j = 0; j < vertexindex.size(); j++)
				{
					float x, y, z, nx, ny, nz;


					x = vertices[vertexindex[j] - 1].x, y = vertices[vertexindex[j] - 1].y, z = vertices[vertexindex[j] - 1].z;
					nx = normals[normalindex[j] - 1].x, ny = normals[normalindex[j] - 1].y, nz = normals[normalindex[j] - 1].z;


					float newx = w_matrix[0][0] * x + w_matrix[0][1] * y + w_matrix[0][2] * z + w_matrix[0][3];
					float newy = w_matrix[1][0] * x + w_matrix[1][1] * y + w_matrix[1][2] * z + w_matrix[1][3];
					float newz = w_matrix[2][0] * x + w_matrix[2][1] * y + w_matrix[2][2] * z + w_matrix[2][3];


					float newnx = w_matrix[0][0] * nx + w_matrix[0][1] * ny + w_matrix[0][2] * nz;
					float newny = w_matrix[1][0] * nx + w_matrix[1][1] * ny + w_matrix[1][2] * nz;
					float newnz = w_matrix[2][0] * nx + w_matrix[2][1] * ny + w_matrix[2][2] * nz;


					glNormal3f(newnx, newny, newnz);
					glVertex3f(newx, newy, newz);

				}
				glEnd();
			}

		}
	}
	glEndList();
}
*/
void IOBJ::render2() {
	for (auto iter = facevector.begin(); iter != facevector.end(); iter++) {
		face faceandmtl = *iter;
		std::string t_mtlname = faceandmtl.mtl;
		std::vector<std::vector<std::vector<unsigned short>>> faces = faceandmtl.Index;

		std::vector<std::vector<unsigned short>> t_vertexIndices, t_uvIndices, t_normalIndices;
		t_vertexIndices = faces[0];
		t_uvIndices = faces[1];
		t_normalIndices = faces[2];

		std::map<std::string, std::vector<float>> usemtl = mtlname[t_mtlname];

		std::vector<float> temp;

		if (usemtl.count("Ka") != 0) {
			temp = usemtl["Ka"];
			GLfloat Ka[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);

		}
		GLfloat Kd[3];
		if (usemtl.count("Kd") != 0)
		{
			temp = usemtl["Kd"];
			Kd[0] = temp[0];
			Kd[1] = temp[1];
			Kd[2] = temp[2];
			glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);

		}

		if (usemtl.count("Ks") != 0)
		{
			temp = usemtl["Ks"];
			GLfloat Ks[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
		}

		if (usemtl.count("Ke") != 0)
		{
			temp = usemtl["Ke"];
			GLfloat Ke[3] = { temp[0], temp[1], temp[2] };
			glMaterialfv(GL_FRONT, GL_EMISSION, Ke);

		}

		if (usemtl.count("Ns") != 0)
		{

			temp = usemtl["Ns"];
			GLfloat Ns = temp[0];


			glMaterialf(GL_FRONT, GL_SHININESS, Ns);
		}




		if (texmap.count(t_mtlname) != 0) {
			std::map<std::string, std::vector<int>> face_tex_size;
			face_tex_size = texmapsize[t_mtlname];

			std::map<std::string, GLubyte*> face_tex;
			face_tex = texmap.at(t_mtlname);

			if (face_tex.count("map_Kd") != 0) {
				std::vector<int> temp_size_vector = face_tex_size["map_Kd"];
				GLubyte* temp_tex = face_tex["map_Kd"];

				glTexImage2D(GL_TEXTURE_2D, 0, 3, temp_size_vector[0], temp_size_vector[1], 0, GL_RGB, GL_UNSIGNED_BYTE, temp_tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}

			glEnable(GL_TEXTURE_2D);

			register unsigned int indexsize = t_vertexIndices.size();

			for (register unsigned int i = 0; i < indexsize; i++) {
				std::vector<unsigned short> vertexindex = t_vertexIndices[i];
				std::vector<unsigned short> uvindex = t_uvIndices[i];
				std::vector<unsigned short> normalindex = t_normalIndices[i];


				register unsigned int vertexsize = vertexindex.size();
				glBegin(GL_POLYGON);
				for (register unsigned int j = 0; j < vertexsize; j++)
				{

					float x, y, z, nx, ny, nz, ux, uy;

					x = vertices[vertexindex[j] - 1].x, y = vertices[vertexindex[j] - 1].y, z = vertices[vertexindex[j] - 1].z;
					nx = normals[normalindex[j] - 1].x, ny = normals[normalindex[j] - 1].y, nz = normals[normalindex[j] - 1].z;
					ux = uvindex[j] - 1, uy = uvindex[j] - 1;


					float newx = w_matrix[0][0] * x + w_matrix[0][1] * y + w_matrix[0][2] * z + w_matrix[0][3];
					float newy = w_matrix[1][0] * x + w_matrix[1][1] * y + w_matrix[1][2] * z + w_matrix[1][3];
					float newz = w_matrix[2][0] * x + w_matrix[2][1] * y + w_matrix[2][2] * z + w_matrix[2][3];

					float newnx = w_matrix[0][0] * nx + w_matrix[0][1] * ny + w_matrix[0][2] * nz;
					float newny = w_matrix[1][0] * nx + w_matrix[1][1] * ny + w_matrix[1][2] * nz;
					float newnz = w_matrix[2][0] * nx + w_matrix[2][1] * ny + w_matrix[2][2] * nz;



					glNormal3f(newnx, newny, newnz);
					glTexCoord2d(uvs[ux].x, uvs[uy].y);
					glVertex3f(newx, newy, newz);
				}
				glEnd();
			}

			glDisable(GL_TEXTURE_2D);
		}
		else {

	//		glColor3f(Kd[0], Kd[1], Kd[2]);
			
			for (register unsigned int i = 0; i < t_vertexIndices.size(); i++) {
				std::vector<unsigned short> vertexindex = t_vertexIndices[i];
				std::vector<unsigned short> normalindex = t_normalIndices[i];


				glBegin(GL_POLYGON);
				for (register unsigned int j = 0; j < vertexindex.size(); j++)
				{
					
					float x, y, z, nx, ny, nz;


					x = vertices[vertexindex[j] - 1].x, y = vertices[vertexindex[j] - 1].y, z = vertices[vertexindex[j] - 1].z;
					nx = normals[normalindex[j] - 1].x, ny = normals[normalindex[j] - 1].y, nz = normals[normalindex[j] - 1].z;


					float newx = w_matrix[0][0] * x + w_matrix[0][1] * y + w_matrix[0][2] * z + w_matrix[0][3];
					float newy = w_matrix[1][0] * x + w_matrix[1][1] * y + w_matrix[1][2] * z + w_matrix[1][3];
					float newz = w_matrix[2][0] * x + w_matrix[2][1] * y + w_matrix[2][2] * z + w_matrix[2][3];


					float newnx = w_matrix[0][0] * nx + w_matrix[0][1] * ny + w_matrix[0][2] * nz;
					float newny = w_matrix[1][0] * nx + w_matrix[1][1] * ny + w_matrix[1][2] * nz;
					float newnz = w_matrix[2][0] * nx + w_matrix[2][1] * ny + w_matrix[2][2] * nz;

					
					glNormal3f(newnx, newny, newnz);
					
					glVertex3f(newx, newy, newz);

				}
				glEnd();
			}
		}
	}
}

void IOBJ::render() {
	glCallList(gl_list);
}

void IOBJ::loadcheck()
{
	//std::cout << mtlname.at("") << '\n';

}

void IOBJ::trnaslate(float x, float y, float z)
{
	w_matrix[0][3] += x;
	w_matrix[1][3] += y;
	w_matrix[2][3] += z;
}

void IOBJ::rotaionx(double r)
{
	r = r * 3.14 / 180.0;
	double temp_matrix[4][4];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp_matrix[i][j] = w_matrix[i][j];
		}
	}

	w_matrix[0][1] = cos(r) * temp_matrix[0][1] - sin(r) * temp_matrix[0][2];
	w_matrix[0][2] = sin(r) * temp_matrix[0][1] + cos(r) * temp_matrix[0][2];

	w_matrix[1][1] = cos(r) * temp_matrix[1][1] - sin(r) * temp_matrix[1][2];
	w_matrix[1][2] = sin(r) * temp_matrix[1][1] + cos(r) * temp_matrix[1][2];

	w_matrix[2][1] = cos(r) * temp_matrix[2][1] - sin(r) * temp_matrix[2][2];
	w_matrix[2][2] = sin(r) * temp_matrix[2][1] + cos(r) * temp_matrix[2][2];
}

void IOBJ::scalematrix(float s)
{
	w_matrix[0][0] *= s;
	w_matrix[1][1] *= s;
	w_matrix[2][2] *= s;


}

void IOBJ::rotaiony(double r)
{
	r = r * 3.14 / 180.0;

	double temp_matrix[4][4];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp_matrix[i][j] = w_matrix[i][j];
		}
	}

	w_matrix[0][0] = cos(r) * temp_matrix[0][0] + sin(r) * temp_matrix[0][2];
	w_matrix[0][2] = -sin(r) * temp_matrix[0][0] + cos(r) * temp_matrix[0][2];

	w_matrix[1][0] = cos(r) * temp_matrix[1][0] + sin(r) * temp_matrix[1][2];
	w_matrix[1][2] = -sin(r) * temp_matrix[1][0] + cos(r) * temp_matrix[1][2];

	w_matrix[2][0] = cos(r) * temp_matrix[2][0] + sin(r) * temp_matrix[2][2];
	w_matrix[2][2] = -sin(r) * temp_matrix[2][0] + cos(r) * temp_matrix[2][2];

}

void IOBJ::rotaionz(double r)
{
	r = r * 3.14 / 180.0;

	double temp_matrix[4][4];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp_matrix[i][j] = w_matrix[i][j];
		}
	}

	w_matrix[0][0] = cos(r) * temp_matrix[0][0] - sin(r) * temp_matrix[0][1];
	w_matrix[0][1] = sin(r) * temp_matrix[0][0] + cos(r) * temp_matrix[0][1];

	w_matrix[1][0] = cos(r) * temp_matrix[1][0] - sin(r) * temp_matrix[1][1];
	w_matrix[1][1] = sin(r) * temp_matrix[1][0] + cos(r) * temp_matrix[1][1];

	w_matrix[2][0] = cos(r) * temp_matrix[2][0] - sin(r) * temp_matrix[2][1];
	w_matrix[2][1] = sin(r) * temp_matrix[2][0] + cos(r) * temp_matrix[2][1];
}

