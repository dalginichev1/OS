#include "DynamicLoader.hpp"

#include <dlfcn.h>

DynamicLoader::DynamicLoader()
    : handle_(nullptr), createDer(nullptr), createSort(nullptr),
      deleteDer(nullptr), deleteSort(nullptr) {}

DynamicLoader::~DynamicLoader()
{
    if (handle_) dlclose(handle_);
}

bool DynamicLoader::Load(const std::string& path)
{
    if (handle_) dlclose(handle_);
    handle_ = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle_)
    {
        lastErr = dlerror();
        return false;
    }

    createDer = (CreateDerivativeFunc)dlsym(handle_, "CreateDerivative");
    createSort = (CreateSortFunc)dlsym(handle_, "CreateSort");
    deleteDer = (DeleteDerivativeFunc)dlsym(handle_, "DeleteDerivative");
    deleteSort = (DeleteSortFunc)dlsym(handle_, "DeleteSort");

    if (!createDer || !createSort || !deleteDer || !deleteSort) {
        lastErr = dlerror();
        dlclose(handle_);
        handle_ = nullptr;
        return false;
    }

    return true;
}

std::string DynamicLoader::Error() const {
    return lastErr;
}

AbsDerivative* DynamicLoader::CreateDerivative() {
    return createDer ? createDer() : nullptr;
}

AbsSort* DynamicLoader::CreateSort() {
    return createSort ? createSort() : nullptr;
}

void DynamicLoader::DeleteDerivative(AbsDerivative* p) {
    if (deleteDer) deleteDer(p);
}

void DynamicLoader::DeleteSort(AbsSort* p) {
    if (deleteSort) deleteSort(p);
}
