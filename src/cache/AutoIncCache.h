// since cachelib does not support `InvalidateAll` operation, 
// we use a global counter as a workaround.

#pragma once 
#include "cachelibHeader.h"
static int gCounter = 0;

void initializeCache()
{
    facebook::cachelib_examples::initializeCache();
}

void destroyCache()
{
    facebook::cachelib_examples::destroyCache();
}

int addpool_(std::string poolName)
{
    return facebook::cachelib_examples::addpool_(poolName);
}

bool set_(cachelib::PoolId pid, CacheKey key, const std::string& value)
{
    return facebook::cachelib_examples::set_(pid, gCounter + "__" + key, value);
}

std::string get_(CacheKey key)
{
    return facebook::cachelib_examples::get_(gCounter + "__" + key);
}

bool del_(CacheKey key)
{
    return facebook::cachelib_examples::del_(gCounter + "__" + key);
}

size_t getAvailableSize()
{
    return facebook::cachelib_examples::getAvailableSize();
}

std::set<PoolId> getPoolIds_()
{
    return facebook::cachelib_examples::getPoolIds_();
}

PoolStats getPoolStat(PoolId pid)
{
    return facebook::cachelib_examples::getPoolStat(pid);
}

void invalidateAll()
{
    gCounter++;
}