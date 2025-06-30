#include <dlfcn.h> 
#include "dynamic_loader.h"

void* loadLibrary(const string& name) { 
    cout << "Пытаемся открыть библиотеку: " << name << "\n";

    void* lib = dlopen(name.c_str(), RTLD_LAZY); if (!lib) { 
        cerr << "Не удалось открыть библиотеку: " << name << " (" << dlerror() << ")\n"; 
    } else { 
        cout << "Библиотека " << name << " успешно загружена\n"; 
    } 
    return lib; 
}

void* getFunction(void* library, const string& name) { 
    return dlsym(library, name.c_str()); 
}

void closeLibrary(void* library) { 
    dlclose(library); 
}
