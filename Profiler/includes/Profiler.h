#pragma once
#include <string>

std::string GetSampleName(int idx);
void BeginProfile();
void BeginSample(size_t id, const std::string& name, const char* filename, int line);
void EndSample(size_t id);
void CalculateInner();
void StoreSample(size_t id, long double percent, long double dt);
void GetStats(size_t id, long double* ave, long double* min, long double* max);
void OutputProfile();
void EndProfile();

class SampleClass
{
public:
    SampleClass(const std::string& name, const char* file, int line)
    {
        size_t id = std::hash<std::string>()(name + std::string(file) + std::to_string(line));
        BeginSample(id, name, file, line);
        m_id = id;
    }

    ~SampleClass()
    {
        EndSample(m_id);
    }

private:
    size_t m_id;
};

#define BEGIN_PROFILE_FRAME() BeginProfile()
#define END_PROFILE_FRAME() EndProfile()
#define SAMPLE() SampleClass sampleInst(__FUNCTION__, __FILE__, __LINE__)
#define PROFILE_OUTPUT() OutputProfile()

