#include <iostream>
#include <vector>
#include <stack>
#include <cassert>
#include <chrono>
#include <string>

class Allocator
{
    unsigned char _data[32];
        
public:
    
    Allocator()
    {
        _data[0]  = 1;
        _data[1]  = 29;
        _data[31] = 0;
    }
    
    unsigned char* Alloc(size_t size)
    {
        unsigned char* cell = _data + 2;
        
        while(cell < _data + 31)
        {
            if(*(cell - 2) == 1 && *(cell - 1) - 2 > size)
            {
                int free_memory = (int)*(cell - 1) - size - 2;
                
                *(cell - 2) = 0;
                *(cell - 1) = size;
                
                *(cell + size) = 1;
                *(cell + 1 + size) = free_memory;
                
                return reinterpret_cast<unsigned char*>(cell);
            }
            else if(*(cell - 2) == 1 && *(cell - 1) < size)
            {
                return nullptr;
            }
            else
            {
                cell += *(cell - 1) + 2;
            }
        }
        return nullptr;
    }
    
    void Delete(unsigned char* object)
    {
        char* ptr_to_delete = reinterpret_cast<char*>(object);
        ptr_to_delete -= 2;
        
        *ptr_to_delete = 1;
    }
    
    void Print()
    {
        unsigned char* ptr = _data;

        while (ptr < _data + 31)
        {
            std::cout << (int)(*ptr);
            
            const int size = *(ptr + 1);
            std::cout << "[" << size << "]";

            for (int i = 0; i < size; i++)
            {
                std::cout << '*';
            }

            ptr += size + 2;
        }

        std::cout << std::endl;
    }
    
    unsigned char* Data() { return _data; }
};

template <class TAllocator>
void StressTest(int count = 10000, int min_size = 1, int max_size = 4)
{
    std::vector<unsigned char*> ptrs;
    TAllocator al;

    for (int i = 0; i < count; i++)
    {
        if (ptrs.empty() || std::rand() % 2 == 0)
        {
            const int size = std::rand() % (max_size - min_size + 1) + min_size;
            std::cout << "Alloc: " << size << std::endl;
            auto ptr = al.Alloc(size);

            if (ptr == nullptr)
            {
                continue;
            }
            ptrs.push_back(ptr);
        }
        else
        {
            const int r = std::rand() % ptrs.size();

            std::cout << "Delete " << (ptrs[r] - al.Data()) << std::endl;
            
            al.Delete(ptrs[r]);

            ptrs.erase(ptrs.begin() + r);
        }

        al.Print();
    }
}

template <class TAllocator>
void Test(std::string s)
{
    auto begin = std::chrono::steady_clock::now();
    StressTest<TAllocator>(3000, 1, 4);
    auto end = std::chrono::steady_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    std::cout << s << " time: " << elapsed_ms.count() << " ms\n";
}

int main()
{
    Test<Allocator>("Allocator");
    return 0;
}
