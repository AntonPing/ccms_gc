#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

struct Data;
typedef struct Cons {
    Data* car;
    Data* cdr;
} Cons;

enum Data_Type {
    cons_t, int_t, real_t, char_t,
    nil_t,
} Data_Type;

union Data_Value {
    Cons cons_v;
    int64_t int_v;
    double real_v;
    char char_v; //原计划支持4字节的unicode
} Data_Value;

typedef struct Data {
    bool is_alive;
    Data_Type type;
    Data_Value value;
} Data;


#define POOL_SIZE 65536
bool use_a_clean_b;
bool gc_finished;
bool halt_for_gc;
Data data_pool_a[POOL_SIZE];
Data* alloc_pool_a[POOL_SIZE];
Data** alloc_ptr_a;
Data** free_ptr_a;

Data data_pool_b[POOL_SIZE];
Data* alloc_pool_b[POOL_SIZE];
Data** alloc_ptr_b;
Data** free_ptr_b;




void initialize() {
    //开始时以a作为分配区，b作为清扫区
    use_a_clean_b = true;
    //因为开始时b区是干净的，所以相当于finished
    gc_finished = true;
    halt_for_gc = false;
    //初始化alloc指针
    alloc_ptr_a = &alloc_pool_a;
    free_ptr_a = &alloc_pool_a;
    alloc_ptr_b = &alloc_pool_b;
    free_ptr_b = &alloc_pool_b;
    //初始化分配池
    for(int i=0; i<POOL_SIZE; i++) {
        alloc_pool_a = &data_pool_a + i;
        alloc_pool_b = &data_pool_b + i;
    }
}

Data* alloc_in_a() {
    //相当于alloc_ptr_a++，但是在指针溢出数组时归位
    alloc_ptr_a = (alloc_ptr_a + 1) mod POOL_SIZE;
    if(alloc_ptr_a == free_ptr_a) {
        //如果alloc追上了free，说明a区空间用完
        if(!gc_finished) { //假如b区未清理完毕
            halt_for_gc = true;
            while(!gc_finished) {
                //sleep(1)
                //等待b区清理完毕
            }
            halt_for_gc = false
        }
        use_a_clean_b = false;
        run_gc_in_a() //把a区进行清扫
    }
    return alloc_ptr_a
}

Data* free_in_a(Data* ptr) {
    free_ptr_a = ptr;
    //相当于free_ptr_a++，但是在指针溢出数组时归位
    free_ptr_a = (free_ptr_a + 1) mod POOL_SIZE;
}

void mark_trash(Data* object) {
    object->is_alive = false;
}

void mark_alive(Data* object) {
    //迭代地标记所有的存活对象
    object->is_alive = true;
    switch(object->type) {
        case cons_t:
            mark_alive(object->value->car);
            mark_alive(object->value->cdr);
            break;
        case int_t:
            break;
        case real_t:
            break;
        case char_t:
            break;
    }
}


void mark_all_a(Data root) {
    Data* reset_ptr = &data_pool_a;
    for(;reset_ptr <= &data_pool_a + POOL_SIZE; reset_ptr++) {
        //将所有对象标记为垃圾
        mark_trash(reset_ptr);
    }
    mark_alive(&root);
}

void sweep_in_a() {
    Data* sweep_ptr = &data_pool_a;
    for(;sweep_ptr <= &data_pool_a + POOL_SIZE; sweep_ptr++) {
        //遍历所有a区对象，清扫垃圾对象
        if(!sweep_ptr->is_alive) {
            free_on_a(sweep_ptr);
        }
    }
}

void run_gc_in_a(Data root) {
    gc_finished = false;
    mark_all_a(root);
    sweep_in_a();
    gc_finished = true;
}

void run_gc_in_b(Data root) {
    gc_finished = false;
    mark_all_b(root);
    sweep_in_b();
    gc_finished = true;
}

Data* alloc() {
    while(halt_for_gc) {
        //sleep(1)
    }
    if(use_a_clean_b) {
        return alloc_in_a()
    } else {
        return alloc_in_b()
    }
}

//测试用例
*Data make_list() {
    *Data new_ptr[8];
    Data_Value temp;

    new_ptr[0] = alloc();
    new_ptr[0]->is_alive = true;
    new_ptr[0]->type = nil_t;

    new_ptr[1] = alloc();
    new_ptr[1]->is_alive = true;
    new_ptr[1]->type = int_t;
    temp.int_v = 4;
    new_ptr[1]->value = temp;

    new_ptr[2] = alloc();
    new_ptr[2]->is_alive = true;
    new_ptr[2]->type = cons_t;
    temp.cons_v = {new_ptr[0],new_ptr[1]};
    new_ptr[2]->value = temp;

    new_ptr[] = alloc();
    new_ptr[2]->is_alive = true;
    new_ptr[2]->type = cons_t;
    temp.cons_v = {new_ptr[0],new_ptr[1]};
    new_ptr[2]->value = temp;


    

}



int main()
{
    printf("Hello, World! \n");

    return 0;
}

