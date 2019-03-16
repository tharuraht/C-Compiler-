#ifndef ast_context_hpp
#define ast_context_hpp

#include <vector>
#include <string>
#include <unordered_map>

class Context {
    public:
    //track local and global variables
    //string is name of variable
    //int is relative location in memory
    std::unordered_map<std::string, unsigned int> globalbindings;
    std::unordered_map<std::string,unsigned int> localbindings;

    bool regavailable[32];
    unsigned int localvar_counter;
    unsigned int globalvar_counter;

    ~Context() {}

    Context() {
        //initially set registers to all available except:
        //0:zero reg & 1
        //2 & 3: return reg
        //26 & 27: kernel reg
        //28: global pointer
        //29: stack pointer
        //30: frame pointer
        //31: return address of subroutine
        for (int i = 0;i < 32;i++) {
            if (i <= 3 || i >= 26)
                regavailable[i] = false;
            else
                regavailable[i] = true;
        }

        localvar_counter = 0;
        globalvar_counter = 0;
    }

    void set_used(unsigned int regno) {
        regavailable[regno] = false;
    }

    void set_unused(unsigned int regno) {
        regavailable[regno] = true;
    }

    std::vector<int> FindFreeRegs (unsigned int first, unsigned int last) {
        std::vector<int> FreeRegs;
        
        for (int i = first; i <= last; i++) {
            if (regavailable[i]) {
                FreeRegs.push_back(i);
            }
        }   

        return FreeRegs;     
    }

    std::vector<int> FreeTempRegs () {
        std::vector<int> FreeTemp;
        std::vector<int> set1 = FindFreeRegs(8,15);
        std::vector<int> set2 = FindFreeRegs(24,25);
        FreeTemp.reserve(set1.size() + set2.size());
        FreeTemp.insert(FreeTemp.end(), set1.begin(), set1.end());
        FreeTemp.insert(FreeTemp.end(), set2.begin(), set2.end());
        return FreeTemp;
    }

    std::vector<int> FreeConstantRegs () {
        return FindFreeRegs(16,23);

    }
    std::vector<int> FreeParamRegs () {
        return FindFreeRegs(4,7);
    }

    void DeleteGlobalVar (std::string varname) {
        globalbindings.erase(varname);
    }

    void NewGlobalVar (std::string varname) {
        globalbindings.emplace(varname,++globalvar_counter);

    }

    void DeleteLocalVar (std::string varname) {
        localbindings.erase(varname);
    }

    void NewLocalVar (std::string varname) {
        //assign a new number to keep track of the variable dec
        localbindings.emplace(varname,++localvar_counter);
    }

    int LookupVariable (std::string varname, int scopelevel) {
        for (int i = scopelevel; i>=0; i--) {
            std::string search_str = varname + std::to_string(scopelevel);
            //scopelevel added as variable may be redeclared in new scope
            if (localbindings.find(search_str) != localbindings.end()) {
                //variable found
                //return the memory location of the variable
                return ((localbindings.find(search_str)->second + 1)*4);
            }
        }
        //if not found return 0
        return 0;
    }
};

#endif