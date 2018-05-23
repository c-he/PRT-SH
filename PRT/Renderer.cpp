#include "Renderer.h"

extern bool simpleLight;
extern int objectIndex;
extern Lighting simpleL;

using glm::mat3;

void Renderer::toGPUObject()
{
	glGenVertexArrays(1,&_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1,&_vboVertex);
	glBindBuffer(GL_ARRAY_BUFFER,_vboVertex);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * _scene->_vertexes.size(),
		_scene->_vertexes.data(),
		GL_STATIC_DRAW
		);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

	glGenBuffers(1,&_vboNormal);
	glBindBuffer(GL_ARRAY_BUFFER,_vboNormal);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * _scene->_normals.size(),
		_scene->_normals.data(),
		GL_STATIC_DRAW
		);

	glGenBuffers(1,&_vboColor);
	glBindBuffer(GL_ARRAY_BUFFER,_vboColor);
	/*glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * _scene->co*/



}

void Renderer::initDiffuseBuffer(int type)
{
	if(_diffObject->band() != _lighting->band())
	{
		std::cout << "different number band!" << std::endl;
	}
	int vertexnumber = _diffObject->_vertexes.size() /3;
	int band2 = _diffObject->band() * _diffObject->band();

	_colorBuffer.clear();
	for(int i = 0;i < vertexnumber; ++i)
	{
		float cr = 0,cg = 0,cb = 0;
		float lightcoeff[3];
		for(int j = 0;j < band2; ++j)
		{
			lightcoeff[0] = _lighting->_Vcoeffs[0](j);
			lightcoeff[1] = _lighting->_Vcoeffs[1](j);
			lightcoeff[2] = _lighting->_Vcoeffs[2](j);

			if(simpleLight)
			{
				for(int k = 0;k < 3; ++k)
					lightcoeff[k] = simpleL._Vcoeffs[k](j);
			}

			cr += lightcoeff[0] * _diffObject->_DTransferFunc[type][i][j][0];
			cg += lightcoeff[1] * _diffObject->_DTransferFunc[type][i][j][1];
			cb += lightcoeff[2] * _diffObject->_DTransferFunc[type][i][j][2];

		}

		cr *= _lighting->HDRaffect().x;
		cg *= _lighting->HDRaffect().y;
		cb *= _lighting->HDRaffect().z;


		_colorBuffer.push_back(cr);
		_colorBuffer.push_back(cg);
		_colorBuffer.push_back(cb);
	}

}

void Renderer::initColorBuffer(int type,vec3 viewDir, bool diffornot = true)
{
	if(diffornot)
		initDiffuseBuffer(type);
	else
		initGeneralBuffer(type,viewDir);
}

void Renderer::initGeneralBuffer(int type,vec3 viewDir)
{
	std::cout <<"type"<< type << std::endl;
	std::cout << "name" << _genObject->_modelname << std::endl;
	if(objectIndex == 0)
	{
		if(type > 1)
			type = 1;
	}
	else
	{
		type = 0;
	}

	std::cout <<"buffer type " << type << std::endl;

	if(_genObject->band() != _lighting->band())
	{
		std::cout << "different number band!" << std::endl;
	}
	int vertexnumber = _genObject->_vertexes.size() /3;
	int band = _genObject->band();
	int band2 = band * band;

	_colorBuffer.clear();
	_colorBuffer.resize(_genObject->_vertexes.size());

	//std::cout << "viewDir"  << viewDir.x << ' '<< viewDir.y << ' ' << viewDir.z << std::endl;
	int validNumber = 0;
#pragma omp parallel for
	for(int i = 0;i < vertexnumber; ++i)
	{
		int offset = 3 * i;
		vec3 position = vec3(_genObject->_vertexes[offset],_genObject->_vertexes[offset+1],_genObject->_vertexes[offset+2]);
		vec3 normal = vec3(_genObject->_normals[offset],_genObject->_normals[offset+1],_genObject->_normals[offset+2]);

	

		float color[3];
		//float lightcoeff[3];

		VectorXf transferedLight[3];
		if(type == 0)
		{
			transferedLight[0] = _genObject->_TransferMatrix[0][i] * _lighting->_Vcoeffs[0];
			transferedLight[1] = _genObject->_TransferMatrix[0][i] * _lighting->_Vcoeffs[1];
			transferedLight[2] = _genObject->_TransferMatrix[0][i] * _lighting->_Vcoeffs[2];
		}
		else
		{
			transferedLight[0] = _genObject->_TransferMatrix[1][i] * _lighting->_Vcoeffs[0];
			transferedLight[1] = _genObject->_TransferMatrix[1][i] * _lighting->_Vcoeffs[1];
			transferedLight[2] = _genObject->_TransferMatrix[1][i] * _lighting->_Vcoeffs[2];

		}

	

		Lighting lightingtemp(band,transferedLight);

		float rotateMatrix[3][3];
		vec3 tangent = vec3(_genObject->_tangent[i].x,_genObject->_tangent[i].y,_genObject->_tangent[i].z);
		vec3 binormal = glm::cross(normal,tangent) * _genObject->_tangent[i].w;
		//vec3 binormal = glm::cross(normal,tangent);

		//TEST DATA 1 PASSED
		/*tangent = vec3(0.0f,0.0f,1.0f);
		normal = vec3(0.0f,1.0f,0.0f);
		binormal = glm::cross(normal,tangent) ;*/


		//TEST DATA 2 PASSED
		/*tangent = vec3(0.0,-1.0f,0.0f);
		normal = vec3(0.0f,0.0f,1.0f);
		binormal = glm::cross(normal,tangent);

		//TEST DATA 3  PASSED
		tangent = vec3(1.0f,0.0f,0.0f);
		normal = vec3(0.0f,1.0f,0.0f);
		binormal = glm::cross(normal,tangent) ;

		//TEST DATA 4 PASSED
		tangent = vec3(0.0f,-1.0f,0.0f);
		normal = vec3(1.0,0.0,0.0);
		binormal = glm::cross(normal,tangent);*/

		//TEST DATA5
		/*tangent = vec3(0.0f,0.0f,1.0f);
		normal = vec3(-1.0f,0.0f,0.0f);
		binormal = glm::cross(normal,tangent);*/

	/*	if(fabs(normal.x + 1.0f) < 0.02f)
		{
			std::cout << "local coord check" << std::endl;

			std::cout << "normal " << normal.x << ' ' << normal.y << ' ' << normal.z<< std::endl;
			std::cout << "tangent " << tangent.x << ' ' << tangent.y << ' ' << tangent.z<< std::endl;
			std::cout << "binormal " << binormal.x << ' ' << binormal.y << ' ' << binormal.z<< std::endl;

			system("pause");



		}*/

		//std::cout << binormal.x << ' '<< binormal.y << ' '<< binormal.z << std::endl;


		mat3 changeCoord;


		/*tangent = vec3(1.0f,0.0f,0.0f);
		normal = vec3(0.0f,0.0f,1.0f);
		binormal = glm::cross(normal,tangent) ;*/

		//yzx(OpenGL) to zxy(not OpenGL)
		vec3 mattangent,matnormal,matbinormal;
		matnormal = vec3(normal.z,normal.x,normal.y);
		mattangent = vec3(tangent.z,tangent.x,tangent.y);
		matbinormal = vec3(binormal.z,binormal.x,binormal.y);

		

		//rotate matrix in zxy
		for(int m = 0;m < 3; ++m)rotateMatrix[m][0] = mattangent[m];
		for(int m = 0;m < 3; ++m)rotateMatrix[m][1] = matbinormal[m];
		for(int m = 0;m < 3; ++m)rotateMatrix[m][2] = matnormal[m];

		float alpha,beta,gamma;

		rotateMatrixtoYZY(rotateMatrix,alpha,beta,gamma);

		vector<vec2> paraResult;
		paraResult.push_back(vec2(gamma,beta));
		paraResult.push_back(vec2(alpha,0.0f));


		lightingtemp.rotateZYZ(paraResult);

		//CONVOLUTION wih BRDF
		for(int l = 0; l < band; ++l)
		{
			float alpha_l_0 = sqrt((4.0f * (float)M_PI)/(2*l + 1));

			int BRDFindex = l*(l + 1);
			for(int m = -l; m <= l; ++m)
			{
				int index = BRDFindex + m;
				for(int k = 0;k < 3; ++k)
				{
					lightingtemp._Vcoeffs[k](index) *= (alpha_l_0 *_genObject->_BRDFcoeff(BRDFindex));
				}
			}
		}

		vec3 dir = viewDir;

		dir = glm::normalize(dir);


		float theta = 0.0f,phi = 0.0f;

		if(glm::dot(normal,dir) < 0)
		{
			_colorBuffer[3 * i + 0] = 0.0f;
			_colorBuffer[3 * i + 1] = 0.0f;
			_colorBuffer[3 * i + 2] = 0.0f;

			continue;
		}

		theta = acos(glm::clamp(glm::dot(normal,dir),-1.0f,1.0f));
		//phi = acos(glm::clamp(glm::dot(normal,dir),-1.0f,1.0f));
	//	std::cout <<"Theta" << theta << std::endl;
		validNumber ++;

		for(int s = 0;s < 3; ++s)
		{
			color[s] = 0.0f;
			for(int l = 0;l < band; ++l)
			{
				for(int m = -l;m <= l;++m)
				{
					int index = l*(l+1) + m;
					color[s] += lightingtemp._Vcoeffs[s](index) * (float)SphericalH::SHvalue(theta,phi,l,m);

				}
			}

			color[s] *= _lighting->Glossyaffect()[s];
			_colorBuffer[3 * i + s] = color[s];
		}
	}

	std::cout << validNumber <<std::endl;
	std::cout << "Color buffer done." << std::endl;
}

void Renderer::naiveObjRender()
{
	int facenumber;
	if(_genObject == NULL)
		facenumber = _diffObject->_renderIndex.size()/3;
	else
		facenumber = _genObject->_renderIndex.size()/3;

	glBegin(GL_TRIANGLES);

	for(int i = 0;i < facenumber; ++i)
	{
		int offset = 3 * i;
		int index[3];
		if(_genObject == NULL)
		{
			index[0] = _diffObject->_renderIndex[offset];
			index[1] = _diffObject->_renderIndex[offset + 1];
			index[2] = _diffObject->_renderIndex[offset + 2];
		}
		else
		{
			index[0] = _genObject->_renderIndex[offset];
			index[1] = _genObject->_renderIndex[offset + 1];
			index[2] = _genObject->_renderIndex[offset + 2];
		}
		
		for(int j = 0;j < 3; ++j)
		{
			int Vindex = 3 * index[j];

			glColor3f(_colorBuffer[Vindex],_colorBuffer[Vindex + 1],_colorBuffer[Vindex + 2]);
			if(_genObject == NULL)
				glVertex3f(_diffObject->_vertexes[Vindex],_diffObject->_vertexes[Vindex + 1],_diffObject->_vertexes[Vindex + 2]);
			else
				glVertex3f(_genObject->_vertexes[Vindex],_genObject->_vertexes[Vindex + 1],_genObject->_vertexes[Vindex + 2]);


		}
	}

	glEnd();
}
