#pragma once
#include "lib.h"

class IOBJ
{
private:

	struct vec3
	{
		float x;
		float y;
		float z;
	};

	struct vec2
	{
		float x;
		float y;
	};

	struct face
	{
		std::string mtl;
		std::vector<std::vector<std::vector<unsigned short>>> Index;
	};

public:
	IOBJ(const char* filename);
	~IOBJ();

	
	bool mcheck;

	std::vector<face> facevector;
	std::vector<vec3> vertices;
	std::vector<vec3> uvs;
	std::vector<vec3> normals;

	/*
	Ka : ambient
	Kd : diffuse
	KS : specular
	KE : Emission
	NS : 반사ㅏ지수
	Ni : 표면 확학 밀도, 굴절률?
	d  : 투명도
	illum : 조명 모델 지정
	*/
	


	GLuint gl_list;


	// mtl, texture
	std::map<std::string, std::map<std::string, std::vector<float>>> mtlname;
	std::map<std::string, std::map<std::string, GLubyte*>> texmap;
	std::map<std::string, std::map<std::string, std::vector<int>>>texmapsize;



	double w_matrix[4][4] = { {1.0, 0.0, 0.0, 0.0},
							 {0.0, 1.0, 0.0, 0.0},
							 {0.0, 0.0, 1.0, 0.0},
							 {0.0 , 0.0, 0.0, 1.0} };

	bool loadFile2(const char* filename);

	bool loadMeterialFile(const char* filename);

	bool loadTexture(const char* filename, std::string mtlname, std::string texname);

	void generate();
	void render2();
	void render();

	void subdivision_triangle();
	void subdivision_triangle_notgen_vertex();

	void makesiple(float theta);

	void trnaslate(float x, float y, float z);
	void rotaionx(double r);
	void rotaiony(double r);
	void rotaionz(double r);
	void scalematrix(float s);


	void loadcheck();
};