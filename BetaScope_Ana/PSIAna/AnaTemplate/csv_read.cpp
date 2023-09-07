#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

class csv_run_reader{
    public:
    
        csv_run_reader(std::string fname){
            std::fstream file (fname, std::ios::in);
            std::vector<std::string> row;
            std::string line, word;
            
            if(file.is_open()){
                while(getline(file, line)){
                    row.clear();
                    std::stringstream str(line);
                    while(getline(str, word, ',')) row.push_back(word);
                    if(row[0] == "Run"){
                        m_header = row;
                        continue;
                    }
                    std::vector<float> test_v;
                    for(std::string el : row) test_v.push_back(std::stof(el));
                    m_run_map[row[0]] = test_v;
                }
            }else{
                std::cout<<"Could not open file"<<std::endl;
            }
            
        }
        
        std::vector<float> run_selections(std::string run_name){
	    //If there is no csv entry for the run_name pick the default value which is run number "-1"
	    if (m_run_map.find(run_name) == m_run_map.end()){
		std::cout<<"Warning: I have no entry in the csv file for this root file"<<std::endl;
		std::string no_entry_handling = "-1";
                return m_run_map[no_entry_handling]; 
	    }
	    else{
		return m_run_map[run_name];
	    }
        }
        
        std::vector<std::string> get_header(){
            return m_header;
        }
        
        ~csv_run_reader(){
            m_run_map.clear();
        }    
    
    private:
    
        std::map<std::string, std::vector<float>> m_run_map;
        std::vector<std::string> m_header;
        
};

//int main(){
//    std::string fname("test.csv");
//  // Load csv file
//    csv_run_reader run_map(fname);
//  
//  
//  // get the csv file header
//    std::cout<<run_map.get_header()[0]<<" "<<run_map.get_header()[1]<<std::endl;
//  
//  
//  // Run is a string, returns a vector of floats as in the file. Argument 0 is the run number as number
//  // it's a map, so no need to have the right order on the csv
//    std::cout<<run_map.run_selections("131")[0]<<" "<<run_map.run_selections("131")[1]<<run_map.run_selections("131")[2]<<run_map.run_selections("131")[3]<<std::endl;
//  
//    return 0;
//}
