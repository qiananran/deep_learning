#include "data_handler.hpp"

data_handler::data_handler()
{
    data_array = new std::vector<data *>;
    test_data = new std::vector<data *>;
    training_data = new std::vector<data *>;
    validation_data = new std::vector<data *>;
}
data_handler::~data_handler()
{
    // FREE Dynamically allocated stu
}

void data_handler::read_feature_vector(std::string path)
{
    uint32_t header[4];
    unsigned char bytes[4];
    FILE *f = fopen(path.c_str(), "r");
    if(f){
        for (int i = 0; i < 4; i++)
        {
            if (fread(bytes, sizeof(bytes), 1,f))
            {
                header[i] = convert_to_little_endian(bytes);
            }
            
        }
        printf("Done getting input file header.\n");
        int image_size = header[2]*header[3];
        for(int i = 0; i < header[1]; i++){
            data *d = new data();
            uint8_t element[1];
            for(int j = 0; j < image_size; j++)
            {
                if(fread(element, sizeof(element), 1, f))
                {
                    d->append_to_feature_vector(element[0]);
            
                }else
                {
                    printf("Erro Reading from file.\n");
                    exit(1);
                }
            }
            data_array->push_back(d);
        }
        printf("Successfully read and stared %lu feature vectors", data_array->size());
    }else
    {
        printf("Could not find file.\n");
        exit(1);
    }
}

void data_handler::read_feature_labels(std::string path){
    uint32_t header[2];
    unsigned char bytes[4];
    FILE *f = fopen(path.c_str(), "r");
    if(f){
        for (int i = 0; i < 2; i++)
        {
            if (fread(bytes, sizeof(bytes), 1,f))
            {
                header[i] = convert_to_little_endian(bytes);
            }
            
        }
        printf("Done getting label file header.\n");
        for(int i = 0; i < header[1]; i++){
        
            uint8_t element[1];
            if (fread(element, sizeof(element), 1, f))
            {
                data_array->at(i)->set_label(element[0]);
            }
            else
            {
                printf("Erro Reading from file.\n");
                exit(1);
            }
        }
        printf("Successfully read and stared label.\n");
    }else
    {
        printf("Could not find file.\n");
        exit(1);
    }
}



void data_handler::split_data()
{
    std::unordered_set<int> used_indexes;
    int train_size = data_array->size() * TRAIN_SET_PERCENT;
    int test_size = data_array->size() * TEST_SET_PERCENT;
    int valid_size = data_array->size() * VALIDATION_PERCENT;

    int count = 0;
    while (count < train_size)
    {
        int rand_index = rand() % data_array->size();
        if(used_indexes.find(rand_index) == used_indexes.end())
        {
            training_data->push_back(data_array->at(rand_index));
            used_indexes.insert(rand_index);
            count++;
        }
    }

    // Test Data
    count = 0;
    while (count < test_size)
    {
        int rand_index = rand() % data_array->size();
        if(used_indexes.find(rand_index) == used_indexes.end())
        {
            test_data->push_back(data_array->at(rand_index));
            used_indexes.insert(rand_index);
            count++;
        }
    }

    // validation data
    count = 0;
    while (count < valid_size)
    {
        int rand_index = rand() % data_array->size();
        if(used_indexes.find(rand_index) == used_indexes.end())
        {
            validation_data->push_back(data_array->at(rand_index));
            used_indexes.insert(rand_index);
            count++;
        }
    }

    printf("Training Data size : %lu.\n", training_data->size());
    printf("test Data size : %lu.\n", test_data->size());
    printf("validation Data size : %lu.\n", validation_data->size());

    
}

void data_handler::count_classes()
{
    int count = 0;
    for (unsigned i = 0; i < data_array->size(); i++)
    {
        /* code */
        if (class_map.find(data_array->at(i)->get_label()) == class_map.end())
        {
            class_map[data_array->at(i)->get_label()] = count;
            data_array->at(i)->set_enumerated_label(count);
            count++;
        }
        
    }
    num_classes = count;
    printf("Successfully Extracted %d Unique Classes.\n", num_classes);
    
}

uint32_t data_handler::convert_to_little_endian(const unsigned char* bytes)
{
    return (uint32_t) ((bytes[0] << 24) |
                       (bytes[1] << 16) |
                       (bytes[2] << 8)  |
                       (bytes[3]));
}

std::vector<data *> * data_handler::get_training_data()
{
    return training_data;
}
std::vector<data *> * data_handler::get_test_data()
{
    return test_data;
}
std::vector<data *> * data_handler::get_validation_data()
{
    return validation_data;
}

int main()
{
    data_handler *dh = new data_handler();
    dh->read_feature_vector("./train-images-idx3-ubyte");

    dh->read_feature_labels("./train-labels-idx1-ubyte");

    dh->split_data();

    dh->count_classes();
}
