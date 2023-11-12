#include "Profiler.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <windows.h>
#include <iomanip>
#include "Stopwatch.h"
#include <cassert>
#include <limits>

#define PROFILE_MAX_SAMPLE_COUNT 10

struct SampleInfo
{
    size_t id = 0;
    std::string name;
    std::string filename;
    int line = 0;
    int calls = 0;
    int open = 0;
    long double start = 0.0f;
    long double end = 0.0f;
    int parent = -1;
    int numParent = 0;
    long double inner = 0.0f;
};

struct SampleStats
{
    size_t id = 0;
    long double average = 0.0;
    long double maximum = 0.0;
    long double minimum = 0.0;
};

std::vector<SampleInfo> Samples(PROFILE_MAX_SAMPLE_COUNT);
std::vector<SampleStats> Stats(PROFILE_MAX_SAMPLE_COUNT);

long double startTime;
long double endTime;

std::string GetSampleName(int idx)
{
    if (idx < 0 || idx >= PROFILE_MAX_SAMPLE_COUNT)
    {
        return "n/a";
    }

    SampleInfo& info = Samples[idx];
    return info.name;
}

void BeginProfile()
{
    size_t idx = 0;
    while (idx < PROFILE_MAX_SAMPLE_COUNT)
    {
        Samples[idx].id = 0;
        Samples[idx].name = "";
        Samples[idx].filename = "";
        Samples[idx].line = 0;
        Samples[idx].calls = 0;
        Samples[idx].open = 0;
        Samples[idx].start = 0.0f;
        Samples[idx].end = 0.0f;
        Samples[idx].parent = -1;
        Samples[idx].numParent = 0;
        Samples[idx].inner = 0.0f;

        idx++;
    }

    startTime = Stopwatch::GetTimeMS();
}

void BeginSample(size_t id, const std::string& name, const char* filename, int line)
{
    size_t idx = 0;
    while (idx < PROFILE_MAX_SAMPLE_COUNT && Samples[idx].id > 0)
    {
        if (Samples[idx].id == id)
        {
            Samples[idx].calls++;
            Samples[idx].open++;
            return;
        }
        idx++;
    }

    if (idx < PROFILE_MAX_SAMPLE_COUNT)
    {
        Samples[idx].id = id;
        Samples[idx].name = name;
        Samples[idx].filename = filename;
        Samples[idx].line = line;
        Samples[idx].calls = 1;
        Samples[idx].open = 1;
        Samples[idx].start = Stopwatch::GetTimeMS();

        int inner = 0;
        int parent = -1;
        int numParents = 0;
        while (inner < PROFILE_MAX_SAMPLE_COUNT && Samples[inner].id > 0)
        {
            if (Samples[inner].open > 0 && Samples[inner].id != Samples[idx].id)
            {
                numParents++;
                if (parent < 0)
                {
                    parent = inner;
                }
                else if (Samples[inner].start >= Samples[parent].start)
                {
                    parent = inner;
                }
            }
            inner++;
        }

        if (parent >= 0)
        {
            Samples[idx].parent = parent;
            Samples[idx].numParent = numParents;
        }

        int sidx = 0;
        while (sidx < PROFILE_MAX_SAMPLE_COUNT && Stats[sidx].id > 0)
        {
            if (Stats[sidx].id == id)
            {
                return;
            }

            sidx++;
        }

        if (sidx < PROFILE_MAX_SAMPLE_COUNT)
        {
            Stats[sidx].id = id;
            Stats[sidx].average = 0.0f;
            Stats[sidx].maximum = 0.0f;
            Stats[sidx].minimum = 1000000.0f;
        }
    }
}

void EndSample(const std::string& name)
{
    size_t id = std::hash<std::string>()(name);
    EndSample(id);
}

void EndSample(size_t id)
{
    size_t idx = 0;
    while (idx < PROFILE_MAX_SAMPLE_COUNT && Samples[idx].id > 0)
    {
        if (Samples[idx].id == id)
        {
            Samples[idx].open--;
            Samples[idx].end = Stopwatch::GetTimeMS();
            return;
        }
        idx++;
    }
}

void CalculateInner()
{
    size_t idx = 0;
    while (idx < PROFILE_MAX_SAMPLE_COUNT && Samples[idx].id > 0)
    {
        int parent = Samples[idx].parent;
        if (parent >= 0)
        {
            long double total = Samples[idx].end - Samples[idx].start;
            Samples[parent].inner += total;
        }

        idx++;
    }
}

void StoreSample(size_t id, long double percent, long double dt)
{
    unsigned int i = 0;
    float oldRatio;
    float newRatio = 0.8f * ((float)dt / 1000.0f);

    if (newRatio > 1.0f)
    {
        newRatio = 1.0f;
    }
    oldRatio = 1.0f - newRatio;

    while (i < PROFILE_MAX_SAMPLE_COUNT && Stats[i].id > 0)
    {
        if (Stats[i].id == id)
        {
            if (percent < Stats[i].minimum)
            {
                Stats[i].minimum = percent;
            }
            else
            {
                Stats[i].minimum = (Stats[i].minimum * oldRatio) + (percent * newRatio);
            }

            if (Stats[i].minimum < 0.0f)
            {
                Stats[i].minimum = 0.0f;
            }

            if (percent > Stats[i].maximum)
            {
                Stats[i].maximum = percent;
            }
            else
            {
                Stats[i].maximum = (Stats[i].maximum * oldRatio) + (percent * newRatio);
            }


            Stats[i].average = Stats[i].minimum + (Stats[i].maximum - Stats[i].minimum) / 2.0;
            return;
        }
        i++;
    }
}

void GetStats(size_t id, long double* ave, long double* min, long double* max)
{
    unsigned int i = 0;
    while (i < PROFILE_MAX_SAMPLE_COUNT && Stats[i].id > 0)
    {
        if (Stats[i].id == id)
        {
            *ave = Stats[i].average;
            *min = Stats[i].minimum;
            *max = Stats[i].maximum;
            return;
        }

        i++;
    }

    *ave = *min = *max = 0.0f;
}

void OutputProfile()
{
    const int columnWidth = 15;
    const int totalWidth = columnWidth * 9;

    std::stringstream title;
    title << std::left << std::setw(columnWidth) << "CALLS";
    title << std::left << std::setw(columnWidth) << "TOTAL";
    title << std::left << std::setw(columnWidth) << "INNER";
    title << std::left << std::setw(columnWidth) << "SELF";
    title << std::left << std::setw(columnWidth) << "AVG";
    title << std::left << std::setw(columnWidth) << "MIN";
    title << std::left << std::setw(columnWidth) << "MAX";
    title << std::left << std::setw(columnWidth) << "SAMPLES";
    title << std::endl;
    OutputDebugStringA(title.str().c_str());

    std::stringstream separator;
    for (int i = 0; i < totalWidth; i++)
    {
        separator << "=";
    }
    separator << std::endl;
    OutputDebugStringA(separator.str().c_str());

    size_t idx = 0;
    while (idx < PROFILE_MAX_SAMPLE_COUNT && Samples[idx].id > 0)
    {
        long double total = Samples[idx].end - Samples[idx].start;
        long double self = total - Samples[idx].inner;

        long double avg, min, max;
        GetStats(Samples[idx].id, &avg, &min, &max);

        std::string filename = Samples[idx].filename + "(" + std::to_string(Samples[idx].line) + ")";

        std::string indentName = Samples[idx].name;
        for (int indent = 0; indent < Samples[idx].numParent; indent++)
        {
            indentName = "|----" + indentName;
        }

        std::stringstream row;
        row << std::left << std::setw(columnWidth) << Samples[idx].calls;
        row << std::left << std::setw(columnWidth) << std::fixed << std::setprecision(2) << total;
        row << std::left << std::setw(columnWidth) << std::fixed << std::setprecision(2) << Samples[idx].inner;
        row << std::left << std::setw(columnWidth) << std::fixed << std::setprecision(2) << self;
        row << std::left << std::setw(columnWidth) << std::fixed << std::setprecision(2) << avg;
        row << std::left << std::setw(columnWidth) << std::fixed << std::setprecision(2) << min;
        row << std::left << std::setw(columnWidth) << std::fixed << std::setprecision(2) << max;
        row << std::left << std::setw(columnWidth * 2) << indentName << filename;
        row << std::endl;
        OutputDebugStringA(row.str().c_str());

        idx++;
    }
}

void EndProfile()
{
    endTime = Stopwatch::GetTimeMS();
    long double elapsed = endTime - startTime;
    CalculateInner();

    size_t idx = 0;
    while (idx < PROFILE_MAX_SAMPLE_COUNT && Samples[idx].id > 0)
    {
        long double total = Samples[idx].end - Samples[idx].start;
        long double self = total - Samples[idx].inner;
        long double percent = 0.0f;

        if (elapsed > 0)
        {
            percent = (self / elapsed) * 100.0f;
        }

        StoreSample(Samples[idx].id, percent, elapsed);
        idx++;
    }
}