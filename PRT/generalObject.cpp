#include "bvhTree.h"
#include "generalObject.h"

void GeneralObject::computeKernel()
{
    vec3 normal = vec3(0.0f, 1.0f, 0.0f);

    Sampler stemp(64);
    stemp.computeSH(_band);
    int band2 = _band * _band;
    int vertexNumber = _vertices.size() / 3;
    int sampleNumber = stemp._samples.size();

    float weight = 4.0f * (float)MY_PI / sampleNumber;

    _BRDFcoeff.resize(band2);
    _BRDFcoeff.setZero();

    for (int i = 0; i < sampleNumber; ++i)
    {
        float value = 0.0f;
        Sample sp = stemp._samples[i];
        float cosine = glm::dot(normal, sp._cartesCoord);
        cosine = fabs(cosine);
        if (cosine > 0)
            value = powf(cosine, _glossiness);

        //value = std::max(stemp._samples[i]._cartesCoord.y,0.0f);
        for (int j = 0; j < band2; ++j)
        {
            _BRDFcoeff(j) += sp._SHvalue[j] * value;
        }
    }

    for (int i = 0; i < band2; ++i)
    {
        _BRDFcoeff(i) = _BRDFcoeff(i) * weight;
    }
}

void GeneralObject::unshadowed(int size, int band2, Sampler* sampler, int type)
{
    bool visiblity = true;
    int sampleNumber = sampler->_samples.size();
    float weight = 4.0f * (float)MY_PI / sampleNumber;

    BVHTree bvht;

    MatrixXf empty(band2, band2);
    _TransferMatrix[0].resize(size, empty);

    if (type > 1)
    {
        bvht.build(*this);
    }

#pragma omp parallel for
    for (int k = 0; k < size; ++k)
    {
        int offset = 3 * k;
        vec3 normal = vec3(_normals[offset], _normals[offset + 1], _normals[offset + 2]);

        for (int li = 0; li < _band; ++li)
            for (int mi = -li; mi <= li; ++mi)
            {
                for (int lj = 0; lj < _band; ++lj)
                    for (int mj = -lj; mj <= lj; ++ mj)
                    {
                        int iindex = li * (li + 1) + mi;
                        int jindex = lj * (lj + 1) + mj;

                        float integral = 0.0f;
                        for (int j = 0; j < sampleNumber; ++j)
                        {
                            Sample stemp = sampler->_samples[j];
                            float geo = glm::dot(normal, stemp._cartesCoord);

                            if (geo <= 0.0f)
                            {
                                continue;
                            }

                            visiblity = true;
                            if (type > 1)
                            {
                                Ray testRay(vec3(
                                                _vertices[offset],
                                                _vertices[offset + 1],
                                                _vertices[offset + 2]),
                                            stemp._cartesCoord);

                                testRay._o += 2 * M_DELTA * normal;
                                visiblity = !bvht.interTest(testRay);
                            }

                            if (!visiblity)
                                continue;

                            //note: geo term is added because it's the special case 
                            //integral += stemp._SHvalue[iindex]*stemp._SHvalue[jindex]*geo ;//yi,zj
                            integral += stemp._SHvalue[iindex] * stemp._SHvalue[jindex] * geo; //yi,zj
                        }
                        integral *= weight;

                        _TransferMatrix[0][k](jindex, iindex) = integral;
                    }
            }
        //std::cout << k << std::endl;
    }
}

void GeneralObject::interReflect(int size, int band2, Sampler* sampler, int type, int bounce = 1)
{
}

void GeneralObject::project2SH(int mode, int band, int sampleNumber, int bounce)
{
    _difforGeneral = true;
    _band = band;

    int size = _vertices.size() / 3;
    int band2 = _band * _band;

    Sampler stemp((unsigned)sqrt(sampleNumber));
    stemp.computeSH(band);


    if (mode == 1)
    {
        unshadowed(size, band2, &stemp, 1);
    }
    else if (mode == 3)
    {
        unshadowed(size, band2, &stemp, 3);
    }
    else if (mode == 7)
    {
        interReflect(size, band2, &stemp, 7, bounce);
    }
}

//B = (N x T) * T_w
void GeneralObject::computeTBN()
{
    int vertexNumber = _vertices.size() / 3;
    int faceNumber = _indices.size() / 3;

    vector<vec3> tan1;
    vector<vec3> tan2;

    tan1.resize(vertexNumber, vec3(0.0f, 0.0f, 0.0f));
    tan2.resize(vertexNumber, vec3(0.0f, 0.0f, 0.0f));

    for (int i = 0; i < faceNumber; ++i)
    {
        int renderIndexoffset = 3 * i;
        int vindex[3];
        int tindex[3];
        glm::vec3 p[3];
        glm::vec2 w[3];

        for (int j = 0; j < 3; ++j)
        {
            vindex[j] = 3 * _indices[renderIndexoffset + j];
            tindex[j] = 2 * _indices[renderIndexoffset + j];
            p[j] = vec3(_vertices[vindex[j]], _vertices[vindex[j] + 1], _vertices[vindex[j] + 2]);
            w[j] = glm::vec2(_texcoords[tindex[j]], _texcoords[tindex[j] + 1]);

            tindex[j] /= 2;
        }

        float x1 = p[1].x - p[0].x;
        float x2 = p[2].x - p[0].x;
        float y1 = p[1].y - p[0].y;
        float y2 = p[2].y - p[0].y;
        float z1 = p[1].z - p[0].z;
        float z2 = p[2].z - p[0].z;

        float s1 = w[1].x - w[0].x;
        float s2 = w[2].x - w[0].x;
        float t1 = w[1].y - w[0].y;
        float t2 = w[2].y - w[0].y;

        if (fabs(s1 * t2 - s2 * t1) <= M_ZERO)
        {
            continue;
        }
        float r = 1.0F / (s1 * t2 - s2 * t1);

        vec3 tan1Temp((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                      (t2 * z1 - t1 * z2) * r);
        vec3 tan2Temp((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                      (s1 * z2 - s2 * z1) * r);

        if (_isnan(tan1Temp.x) || _isnan(tan1Temp.y) || _isnan(tan1Temp.z) || _isnan(tan2Temp.x) || _isnan(tan2Temp.y)
            || _isnan(tan2Temp.z))
        {
            //system("pause");
            continue;
        }


        tan1[tindex[0]] += tan1Temp;
        tan1[tindex[1]] += tan1Temp;
        tan1[tindex[2]] += tan1Temp;

        tan2[tindex[0]] += tan2Temp;
        tan2[tindex[1]] += tan2Temp;
        tan2[tindex[2]] += tan2Temp;
    }
    _tangent.resize(vertexNumber, vec4(0, 0, 0, 1));
    for (int i = 0; i < vertexNumber; ++i)
    {
        int offset = 3 * i;
        vec3 t = tan1[i];
        vec3 n = vec3(_normals[offset], _normals[offset + 1], _normals[offset + 2]);

        vec3 result = t - n * glm::dot(n, t);
        if (fabs(result.x * result.x + result.y * result.y + result.z * result.z) <= M_ZERO)
        {
            _tangent[i] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
            //	std::cout << "tangent zero" << std::endl;
            //	system("pause");
        }
        else
            _tangent[i] = vec4(glm::normalize(result), 1.0f);

        if (glm::dot(glm::cross(n, t), tan2[i]) < 0.0f)
            _tangent[i].w = -1.0f;
    }

    //std::cout << "debug" << std::endl;
}

void GeneralObject::write2Disk(std::string filename)
{
    std::ofstream out(filename, std::ofstream::binary);
    int size = _vertices.size() / 3;
    int band2 = _band * _band;

    out.write((char *)&size, sizeof(int));
    out.write((char *)&_band, sizeof(int));

    //out << _vertexes.size() << endl;
    //out << _band << endl;

    for (int k = 0; k < size; ++k)
    {
        for (int i = 0; i < band2; ++i)
        {
            for (int j = 0; j < band2; ++j)
            {
                //out << _TransferMatrix[k](i,j) << ' ';
                out.write((char *)&_TransferMatrix[0][k](i, j), sizeof(float));
            }
            //out << endl;
        }
        //out << endl;
    }

    out.close();
    std::cout << "Write Done" << std::endl;
}

void GeneralObject::readFDisk(std::string filename)
{
    std::string transsf[2] = {"G01.dat", "GS01.dat"};

    for (int i = 0; i < 2; ++i)
        _TransferMatrix[i].clear();

    int number = (_modelname == "Scene/buddha.obj") ? 2 : 1;

    for (int s = 0; s < number; ++s)
    {
        std::string temp = filename + transsf[s];

        std::ifstream in(temp, std::ifstream::binary);
        assert(in);
        //std::ifstream in(temp);

        int size, band2;

        in.read((char *)&size, sizeof(int));
        in.read((char *)&_band, sizeof(int));
        std::cout << "size" << size << std::endl;
        std::cout << "band" << _band << std::endl;

        //in >> size >> _band;
        band2 = _band * _band;

        MatrixXf empty(band2, band2);

        for (int k = 0; k < size; ++k)
        {
            _TransferMatrix[s].push_back(empty);

            for (int i = 0; i < band2; ++i)
            {
                for (int j = 0; j < band2; ++j)
                {
                    //			in >> _TransferMatrix[k](i,j);
                    in.read((char *)&_TransferMatrix[s][k](i, j), sizeof(float));
                }
            }
        }
        in.close();
    }


    computeKernel();

    computeTBN();
};
