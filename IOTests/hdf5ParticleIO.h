#ifndef _H5_PARTICLE_IO_H
#define _H5_PARTICLE_IO_H

#include "simIO.h"
#include "hdf5.h"
#include "H5hut.h"

#include <iostream>
#include <mpi.h>


namespace Flecsi_Sim_IO
{


class HDF5ParticleIO: public SimIO
{
    h5_file_t *dataFile;
    int myRank, numRanks;

    


  public:
    void readDatasetAttributeArray(std::string _name, std::string _dataType, void *_data);
    void getDatasetAttributeInfo(int _index, std::string &_varName, std::string &_type, int &numElems);

    HDF5ParticleIO():SimIO(){ dataFile=NULL; }
    HDF5ParticleIO(std::string _fileName, Operation op, MPI_Comm _comm);
    ~HDF5ParticleIO(){ closeFile(); }


    // Open / Close
    int createDataset(std::string _outputFileName, MPI_Comm _comm);
    int openFile(std::string inputFileName, MPI_Comm _comm);

    int openFile(MPI_Comm _comm);
    void closeFile();

    void initMPI(MPI_Comm _comm);


    // Reading
    void readDatasetAttributes();
    void readTimestepInfo(int ts, Timestep &_ts);

    

    
    int getNumParticles(int ts);
    int getNumTimestepAttributes(int ts);
    int getNumVariables(int ts);


    void getTimestepAttributeInfo(int _index, std::string &_varName, std::string &_type, int &numElems);

    template <class T>
    T readTimestepAttribute(std::string _name, std::string _dataType);
    void readTimestepAttributeArray(std::string _name, std::string _dataType, void *_data);
    

    void getTimestepVariableInfo(int _index, std::string &_varName, std::string &_type, int &numElems);

    int readVariables(int ts, std::string _name);
    int readAllVariables(int ts);
    int readVariable(std::string & _name, std::string type, void *_data);




    void displayStepAttributes(int ts);
    void displayFileAttributes();




    // Writing 
    void setTimeStep(int ts){ H5SetStep(dataFile, ts); }

    template <class T>
    int writeDatasetAttribute(std::string _name, std::string _dataType, T _data);
    int writeDatasetAttributeArray(std::string _name, std::string _dataType, void *_data, int numElements=1);

    template <class T>
    int writeTimestepAttribute(std::string _name, std::string _dataType, T _data);
    int writeTimestepAttributeArray(std::string _name, std::string _dataType, void * _data, int numElements=1);

    int writeTimestepAttributes();
    int writeVariables();
};





inline HDF5ParticleIO::HDF5ParticleIO(std::string _fileName, Operation op, MPI_Comm _comm):SimIO(_fileName)
{ 
    if (op == WRITING)
        createDataset(_fileName, _comm); 
    else
        openFile(_fileName, _comm); 
}



inline int HDF5ParticleIO::createDataset(std::string _outputFileName, MPI_Comm _comm)
{ 
    MPI_Comm_rank(_comm, &myRank);
    MPI_Comm_size(_comm, &numRanks);

    outputFileName = _outputFileName;
    dataFile = H5OpenFile(outputFileName.c_str(), H5_O_RDWR, _comm);
    if (!dataFile)
        return -1;

    return 0;
}


inline int HDF5ParticleIO::openFile(std::string inputFileName, MPI_Comm _comm)
{
    MPI_Comm_rank(_comm, &myRank);
    MPI_Comm_size(_comm, &numRanks);

    dataFile = H5OpenFile(inputFileName.c_str(), H5_O_RDONLY, _comm);
    if (!dataFile)
        return -1;

    numTimesteps = H5GetNumSteps(dataFile);
    numDatasetAttributes = H5GetNumFileAttribs(dataFile);
    readDatasetAttributes();

    return 0;
}


inline int HDF5ParticleIO::openFile(MPI_Comm  _comm)
{
    dataFile = H5OpenFile(outputFileName.c_str(), H5_O_RDWR, _comm);
    if (!dataFile)
        return -1;

    return 0;
}


inline void HDF5ParticleIO::initMPI(MPI_Comm _comm)
{
    MPI_Comm_rank(_comm, &myRank);
    MPI_Comm_size(_comm, &numRanks);
}




inline void HDF5ParticleIO::readDatasetAttributes()
{
    if (numDatasetAttributes == 0)      // in case that didn't happen before
        numDatasetAttributes = H5GetNumFileAttribs(dataFile);


    for (int i=0; i<numDatasetAttributes; i++)
    {
        Attribute _x;
        getDatasetAttributeInfo(i, _x.name, _x.dataType, _x.numElements);

        if (_x.dataType == "int32_t")
        {
            _x.data = new int32_t[_x.numElements];
            readDatasetAttributeArray(_x.name, _x.dataType, (int32_t *)_x.data);
        }
        else if (_x.dataType == "int64_t")
        {
            _x.data = new int64_t[_x.numElements];
            readDatasetAttributeArray(_x.name, _x.dataType, (int64_t *)_x.data);
        }
        else if (_x.dataType == "float")
        {
            _x.data = new float[_x.numElements];
            readDatasetAttributeArray(_x.name, _x.dataType, (float *)_x.data);
        }
        else if (_x.dataType == "double")
        {
            _x.data = new double[_x.numElements];
            readDatasetAttributeArray(_x.name, _x.dataType, (double *)_x.data);
        }
        else if (_x.dataType == "string")
        {
            _x.data = new char[_x.numElements];
            readDatasetAttributeArray(_x.name, _x.dataType, (char *)_x.data);
        }

        datasetAttributes.push_back(_x);
    }
}



inline void HDF5ParticleIO::readTimestepInfo(int ts, Timestep &_ts)
{
    // Set timestep
    H5SetStep(dataFile, ts);
    _ts.numAttributes = H5GetNumStepAttribs(dataFile);


    // Read attributes of timestep
    for (int i=0; i<_ts.numAttributes; i++)
    {
        Attribute _x;
        getTimestepAttributeInfo(i, _x.name, _x.dataType, _x.numElements);

        if (_x.dataType == "int32_t")
        {
            _x.data = new int32_t[_x.numElements];
            readTimestepAttributeArray(_x.name, _x.dataType, (int32_t *)_x.data);
        }
        else if (_x.dataType == "int64_t")
        {
            _x.data = new int64_t[_x.numElements];
            readTimestepAttributeArray(_x.name, _x.dataType, (int64_t *)_x.data);
        }
        else if (_x.dataType == "float")
        {
            _x.data = new float[_x.numElements];
            readTimestepAttributeArray(_x.name, _x.dataType, (float *)_x.data);
        }
        else if (_x.dataType == "double")
        {
            _x.data = new double[_x.numElements];
            readTimestepAttributeArray(_x.name, _x.dataType, (double *)_x.data);
        }
        else if (_x.dataType == "string")
        {
            _x.data = new char[_x.numElements];
            readTimestepAttributeArray(_x.name, _x.dataType, (char *)_x.data);
        }

        _ts.attributes.push_back(_x);
    }

    // Read 
    _ts.numVariables = H5PartGetNumDatasets(dataFile); 
    for (int i=0; i<_ts.numVariables; i++) 
    {
        Variable _v;
        getTimestepVariableInfo(i, _v.name, _v.dataType, _v.numElements);
        _ts.vars.push_back(_v);
    }
}



inline void HDF5ParticleIO::closeFile()
{ 
    if (dataFile!=NULL)
    {
        H5CloseFile(dataFile);
        dataFile=NULL;
    }
}





inline int HDF5ParticleIO::getNumParticles(int ts)
{
    H5SetStep(dataFile, ts);
    return H5PartGetNumParticles(dataFile);
}


inline int HDF5ParticleIO::getNumTimestepAttributes(int ts)
{
    H5SetStep(dataFile, ts);
    return H5GetNumStepAttribs(dataFile);
}




inline int HDF5ParticleIO::getNumVariables(int ts)
{
    H5SetStep(dataFile, ts);
    return H5PartGetNumDatasets(dataFile);
}



inline void HDF5ParticleIO::readDatasetAttributeArray(std::string _name, std::string _dataType, void *_data)
{
    if ( _dataType == "int32_t" )
    {
        H5ReadFileAttribInt32(dataFile, _name.c_str(), (int32_t *)_data);
    }
    else if ( _dataType == "int64_t" )
    {
        H5ReadFileAttribInt64(dataFile, _name.c_str(), (int64_t *)_data);
    }
    else if ( _dataType == "float" )
    {
        H5ReadFileAttribFloat32(dataFile, _name.c_str(), (float *)_data);
    }
    else if ( _dataType == "double" )
    {
        H5ReadFileAttribFloat64(dataFile, _name.c_str(), (double *)_data);
    }
    else if ( _dataType == "string" )
    {
        H5ReadFileAttribString(dataFile, _name.c_str(), (char *)_data);
    }
}




template <class T>
inline T HDF5ParticleIO::readTimestepAttribute(std::string _name, std::string _dataType)
{

    if ( _dataType == "int32_t" )
    {
        int32_t _temp;
        H5ReadStepAttribInt32(dataFile, _name.c_str(), &_temp);
        return _temp;
    }
    else if ( _dataType == "int64_t" )
    {
        int64_t _temp;
        H5ReadStepAttribInt64(dataFile, _name.c_str(), &_temp);
        return _temp;
    }
    else if ( _dataType == "float" )
    {
        float _temp;
        H5ReadStepAttribFloat32(dataFile, _name.c_str(), &_temp);
        return _temp;
    }
    else if ( _dataType == "double" )
    {
        double _temp;
        H5ReadStepAttribFloat64(dataFile, _name.c_str(), &_temp);
        return _temp;
    }
}

inline void HDF5ParticleIO::readTimestepAttributeArray(std::string _name, std::string _dataType, void *_data)
{
    if ( _dataType == "int32_t" )
    {
        H5ReadStepAttribInt32(dataFile, _name.c_str(), (int32_t *)_data);
    }
    else if ( _dataType == "int64_t" )
    {
        H5ReadStepAttribInt64(dataFile, _name.c_str(), (int64_t *)_data);
    }
    else if ( _dataType == "float" )
    {
        H5ReadStepAttribFloat32(dataFile, _name.c_str(), (float *)_data);
    }
    else if ( _dataType == "double" )
    {
        H5ReadStepAttribFloat64(dataFile, _name.c_str(), (double *)_data);
    }
    else if ( _dataType == "string" )
    {
        H5ReadStepAttribString(dataFile, _name.c_str(), (char *)_data);
    }
}



inline int HDF5ParticleIO::readVariable(std::string & _name, std::string type, void *_data)
{
    int numParticles = H5PartGetNumParticles(dataFile);
    int start = numParticles/numRanks * myRank;
    H5PartSetView(dataFile, start, -1);

    if ( type == "float" )
        H5PartReadDataFloat32(dataFile, (_name).c_str(), (float *)_data);
    else if ( type == "double" )
        H5PartReadDataFloat64(dataFile, (_name).c_str(), (double *)_data);
    else if ( type == "int32_t" )
        H5PartReadDataInt32(dataFile, (_name).c_str(), (int32_t *)_data);
    else if ( type == "int64_t" )
        H5PartReadDataInt64(dataFile, (_name).c_str(), (int64_t *)_data);
    else
        return -1;

    return 0;
}


inline int HDF5ParticleIO::readAllVariables(int ts)
{
    int retValue;
    for (size_t i=0; i<vars.size(); i++)
        retValue = readVariable(vars[i].name, vars[i].dataType, vars[i].data);

    return retValue;
}


inline int HDF5ParticleIO::readVariables(int ts, std::string _name)
{
    int retValue;
    for (size_t i=0; i<vars.size(); i++)
        if (vars[i].name == _name)
            retValue = readVariable(vars[i].name, vars[i].dataType, vars[i].data);

    return retValue;
}









void HDF5ParticleIO::displayStepAttributes(int ts)
{
    H5SetStep(dataFile, ts);

    const int MAX_LEN = 256;
    h5_int64_t type;
    h5_size_t numElem;
    int numTimestepAttributes = H5GetNumStepAttribs(dataFile);
    for (int i=0; i<numTimestepAttributes; i++)
    {
        h5_int64_t type;
        char fileAttrib[MAX_LEN];

        H5GetStepAttribInfo(dataFile, i, fileAttrib, MAX_LEN, &type, &numElem);
        std::cout << fileAttrib << " " << numElem << " ";
        if (type == H5_INT64_T)
        {
            std::cout << "H5_INT64_T" << std::endl;
        }
        else if (type == H5_INT32_T)
        {
            std::cout << "H5_INT32_T" << std::endl;
        }
        else if (type == H5_FLOAT64_T)
        {
            std::cout << "H5_INT64_T" << std::endl;
        }
        else if (type == H5_FLOAT32_T)
        {
            std::cout << "H5_FLOAT32_T" << std::endl;
        }
        else if (type == H5_STRING_T)
        {
            std::cout << "H5_STRING_T" << std::endl;
        }
        else
        {
            std::cout << "unknown!!!" << std::endl;
        }
    }
}

void HDF5ParticleIO::displayFileAttributes()
{
    const int MAX_LEN = 256;
    h5_int64_t type;
    h5_size_t numElem;
    int numDatasetAttributes = H5GetNumFileAttribs(dataFile);
    for (int i=0; i<numDatasetAttributes; i++)
    {
        h5_int64_t type;
        char fileAttrib[MAX_LEN];

        H5GetFileAttribInfo(dataFile, i, fileAttrib, MAX_LEN, &type, &numElem);
        std::cout << fileAttrib << " " << numElem << " ";
        if (type == H5_INT64_T)
        {
            std::cout << "H5_INT64_T" << std::endl;
        }
        else if (type == H5_INT32_T)
        {
            std::cout << "H5_INT32_T" << std::endl;
        }
        else if (type == H5_FLOAT64_T)
        {
            std::cout << "H5_INT64_T" << std::endl;
        }
        else if (type == H5_FLOAT32_T)
        {
            std::cout << "H5_FLOAT32_T" << std::endl;
        }
        else if (type == H5_STRING_T)
        {
            std::cout << "H5_STRING_T" << std::endl;
        }
        else
        {
            std::cout << "unknown!!!" << std::endl;
        }
    }
}



void HDF5ParticleIO::getDatasetAttributeInfo(int _index, std::string &_varName, std::string &_type, int &numElems)
{
    const int MAX_LEN = 256;
    char varName[MAX_LEN];
    h5_int64_t type;
    h5_size_t numElem;

    H5GetFileAttribInfo(dataFile, _index, varName, MAX_LEN, &type, &numElem);
    _varName = std::string (varName);

    if (type == H5_INT64_T)
        _type = "int64_t";
    else if (type == H5_INT32_T)
        _type = "int32_t";
    else if (type == H5_FLOAT64_T)
        _type = "double";
    else if (type == H5_FLOAT32_T)
        _type = "float";
    else if (type == H5_STRING_T)
        _type = "string";
    else
        _type = "";

    numElems = (int)numElem;
}



void HDF5ParticleIO::getTimestepAttributeInfo(int _index, std::string &_varName, std::string &_type, int &numElems)
{
    const int MAX_LEN = 256;
    char varName[MAX_LEN];
    h5_int64_t type;
    h5_size_t numElem;

    H5GetStepAttribInfo(dataFile, _index, varName, MAX_LEN, &type, &numElem);
    _varName = std::string (varName);

    if (type == H5_INT64_T)
        _type = "int64_t";
    else if (type == H5_INT32_T)
        _type = "int32_t";
    else if (type == H5_FLOAT64_T)
        _type = "double";
    else if (type == H5_FLOAT32_T)
        _type = "float";
    else if (type == H5_STRING_T)
        _type = "string";
    else
        _type = "";

    numElems = (int)numElem;
}


void HDF5ParticleIO::getTimestepVariableInfo(int _index, std::string &_varName, std::string &_type, int &numElems)
{
    const int MAX_LEN = 256;
    char varName[MAX_LEN];
    h5_int64_t type;
    h5_size_t numElem;

    H5PartGetDatasetInfo(dataFile, _index, varName, MAX_LEN, &type, &numElem);
    _varName = std::string (varName);

    if (type == H5_INT64_T)
        _type = "int64_t";
    else if (type == H5_INT32_T)
        _type = "int32_t";
    else if (type == H5_FLOAT64_T)
        _type = "double";
    else if (type == H5_FLOAT32_T)
        _type = "float";
    else if (type == H5_STRING_T)
        _type = "string";
    else
        _type = "";

    numElems = (int)numElem;
}



template <class T>
inline int HDF5ParticleIO::writeDatasetAttribute(std::string _name, std::string _dataType, T _data)
{
    if ( _dataType == "int32_t" )
    {
        int32_t data = (int32_t)_data;
        H5WriteFileAttribInt32(dataFile, _name.c_str(), &data, 1);
    }
    else if ( _dataType == "int64_t" )
    {
        int64_t data = (int64_t)_data;
        H5WriteFileAttribInt64(dataFile, _name.c_str(), &data, 1);
    }
    else if ( _dataType == "float" )
    {
        float data = (float)_data;
        H5WriteFileAttribFloat32(dataFile, _name.c_str(), &data, 1);
    }
    else if ( _dataType == "double" )
    {
        double data = (double)_data;
        H5WriteFileAttribFloat64(dataFile, _name.c_str(), &data, 1);
    }
    else
        return -1;

    return 0;
}

inline int HDF5ParticleIO::writeDatasetAttributeArray(std::string _name, std::string _dataType, void *_data, int numElements)
{
    if ( _dataType == "int32_t" )
    {
        H5WriteFileAttribInt32(dataFile, _name.c_str(), (int32_t *)_data, numElements);
    }
    else if ( _dataType == "int64_t" )
    {
        H5WriteFileAttribInt64(dataFile, _name.c_str(), (int64_t *)_data, numElements);
    }
    else if ( _dataType == "float" )
    {
        H5WriteFileAttribFloat32(dataFile, _name.c_str(), (float *)_data, numElements);
    }
    else if ( _dataType == "double" )
    {
        H5WriteFileAttribFloat64(dataFile, _name.c_str(), (double *)_data, numElements);
    }
    else if ( _dataType == "string" )
    {
        H5WriteFileAttribString(dataFile, _name.c_str(), (char *)_data);
    }
    else
        return -1;
}



template <class T>
inline int HDF5ParticleIO::writeTimestepAttribute(std::string _name, std::string _dataType, T _data)
{
    if ( _dataType == "int32_t" )
    {
        int32_t data = (int32_t)_data;
        H5WriteStepAttribInt32(dataFile, _name.c_str(), &data, 1);
    }
    else if ( _dataType == "int64_t" )
    {
        int64_t data = (int64_t)_data;
        H5WriteStepAttribInt64(dataFile, _name.c_str(), &data, 1);
    }
    else if ( _dataType == "float" )
    {
        float data = (float)_data;
        H5WriteStepAttribFloat32(dataFile, _name.c_str(), &data, 1);
    }
    else if ( _dataType == "double" )
    {
        double data = (double)_data;
        H5WriteStepAttribFloat64(dataFile, _name.c_str(), &data, 1);
    }
    else 
        return -1;
}

inline int HDF5ParticleIO::writeTimestepAttributeArray(std::string _name, std::string _dataType, void * _data, int numElements)
{
    if ( _dataType == "int32_t" )
    {
        H5WriteStepAttribInt32(dataFile, _name.c_str(), (int32_t *)_data, numElements);
    }
    else if ( _dataType == "int64_t" )
    {
        H5WriteStepAttribInt64(dataFile, _name.c_str(), (int64_t *)_data, numElements);
    }
    else if ( _dataType == "float" )
    {
        H5WriteStepAttribFloat32(dataFile, _name.c_str(), (float *)_data, numElements);
    }
    else if ( _dataType == "double" )
    {
        H5WriteStepAttribFloat64(dataFile, _name.c_str(), (double *)_data, numElements);
    }
    else if ( _dataType == "string" )
    {
        H5WriteStepAttribString(dataFile, _name.c_str(), (char *)_data);
    }
    else 
        return -1;
}



inline int HDF5ParticleIO::writeTimestepAttributes()
{
    for (size_t i=0; i<timestepAttributes.size(); i++)
    {
        if ( timestepAttributes[i].dataType == "int32_t" )
            H5WriteStepAttribInt32(dataFile, (timestepAttributes[i].name).c_str(), (int32_t *)timestepAttributes[i].data, timestepAttributes[i].numElements);
        else if ( timestepAttributes[i].dataType == "int64_t" )
            H5WriteStepAttribInt64(dataFile, (timestepAttributes[i].name).c_str(), (int64_t *)timestepAttributes[i].data, timestepAttributes[i].numElements);
        else if ( timestepAttributes[i].dataType == "float" )
            H5WriteStepAttribFloat32(dataFile, (timestepAttributes[i].name).c_str(), (float *)timestepAttributes[i].data, timestepAttributes[i].numElements);
        else if ( timestepAttributes[i].dataType == "double" )
            H5WriteStepAttribFloat64(dataFile, (timestepAttributes[i].name).c_str(), (double *)timestepAttributes[i].data, timestepAttributes[i].numElements);
        else
            return -1;
    }

    timestepAttributes.clear();
    return 0;
}


inline int HDF5ParticleIO::writeVariables()
{
    for (size_t i=0; i<vars.size(); i++)
    {
        H5PartSetNumParticles(dataFile, vars[i].numElements);
        
        if ( vars[i].dataType == "float" )
            H5PartWriteDataFloat32(dataFile, (vars[i].name).c_str(), (float *)vars[i].data);
        else if ( vars[i].dataType == "double" )
            H5PartWriteDataFloat64(dataFile, (vars[i].name).c_str(), (double *)vars[i].data);
        else if ( vars[i].dataType == "int32_t" )
            H5PartWriteDataInt32(dataFile, (vars[i].name).c_str(), (int32_t *)vars[i].data);
        else if ( vars[i].dataType == "int64_t" )
            H5PartWriteDataInt64(dataFile, (vars[i].name).c_str(), (int64_t *)vars[i].data);
        else
            return -1;
    }

    vars.clear();
    return 0;
}




} // end Flecsi_Sim_IO namespace

#endif