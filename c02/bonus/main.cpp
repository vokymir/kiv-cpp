#include <cstddef>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

// 1) implementujte RAII obal nad souborem ze standardni knihovny C
class RAII_File
{
private:
    FILE *fptr = nullptr;
    std::string m_filename = "";

public:
    RAII_File(std::string a_filename){
        m_filename = a_filename;
        fptr = fopen(m_filename.c_str(), "r+");
        if (fptr == nullptr){
            std::cout << "File "+ m_filename + " doesn't exist => creating file." << std::endl;
            fptr = fopen(m_filename.c_str(), "w+");
        }
    }

    ~RAII_File(){
        if (Is_Open()){
            fclose(fptr);
            fptr = nullptr;
        }
    }

    bool Is_Open() { return fptr != nullptr; }
    std::string Get_Filename() { return m_filename; }

    // 2) doplnte metodu Read_Line
    std::string Read_Line()
    {
        if (!Is_Open()){
            std::cout << "Cannot read from non-existing file: " + m_filename << std::endl;
            return "";
        }
        if (feof(fptr)){
            std::cout << "End of file (" + m_filename + ") was already reached." << std::endl;
            return "";
        }

        char c_str[100];

        if(fgets(c_str, sizeof(c_str), fptr) == nullptr){
            if (feof(fptr)){
                std::cout << "End of file (" + m_filename + ") was already reached." << std::endl;
            }
            else if (ferror(fptr)){
                std::cout << "An error occured when reading line in file: " + m_filename << std::endl;
            }
            else{
                std::cout << "Something very bad happened when reading line in file: " + m_filename << std::endl;
            }
            return "";
        }

        std::string s = c_str;

        // strip from \n further because we know its line, don't need \n OR make noise
        size_t pos = s.find('\n');
        if (pos != std::string::npos) {
            s = s.substr(0, pos);
        } else {
            s += "... (line is longer than " + std::to_string(sizeof(c_str) - 1) + " characters)\n";
        }


        return s;
    }

    // 3) doplnte metodu Write_Line
    void Write_Line(const std::string& line)
    {
        if (!Is_Open()){
            std::cout << "Cannot write to non-existing file: " + m_filename << std::endl;
            return;
        }

        const char *c_str = line.c_str();
        if(fputs(c_str, fptr) == EOF){
            std::cout << "Error when writing line into file: " + m_filename << std::endl;
            return;
        }
        // write a newline, because of the name of function
        if(fputc('\n', fptr) == EOF){
            std::cout << "Error when finishing writing line into file: " + m_filename << std::endl;
            return;
        }

        fflush(fptr);
    }
};

// 4) implementujte metodu, ktera vygeneruje obsah do nekolika souboru
void Generate_Files()
{
	// a) vytvorte vektor souboru
	// b) vlozte do nej nekolik instanci RAII_File souboru, do kterych budeme zapisovat
	// c) vygenerujte nejaky obsah, ktery do souboru budete vkladat;
	//    - kazdy soubor by mel mit ruzny obsah
	//    - pristupujte ke vsem souborum v cyklu nad vektorem
    std::vector<std::unique_ptr<RAII_File>> files;
    std::string line = "Ahoj svete.";

    for (int i = 0; i < 5; ++i){
        files.push_back(std::make_unique<RAII_File>("f" + std::to_string(i)));
    }

    for (auto& file : files){
        file->Write_Line(file->Get_Filename());
        file->Write_Line(line);
    }
}

// 5) implementujte metodu, ktera overi, ze v souborech je skutecne ten obsah, ktery tam ma byt
bool Verify_Files()
{
	// a) vytvorte vektor souboru
	// b) vlozte do nej nekolik instanci RAII_File souboru, ze kterych budeme cist
	// c) overte, ze v souborech je to, co jste do nich v Generate_Files zapsali
	//    - vrate true, pokud ano, jinak vratte false
    std::vector<std::unique_ptr<RAII_File>> files;
    std::string line = "Ahoj svete.";

    for (int i = 0; i < 5; ++i){
        files.push_back(std::make_unique<RAII_File>("f" + std::to_string(i)));
    }

    for (auto& file : files){
        std::string line1, line2;
        line1 = file->Read_Line();
        line2 = file->Read_Line();
        if (!(line1 == file->Get_Filename() && line2 == line)) { return false; }
    }
	return true;
}

int main(int argc, char** argv)
{
	Generate_Files();

	const bool outcome = Verify_Files();

	if (outcome)
		std::cout << "Vse probehlo v poradku" << std::endl;
	else
		std::cerr << "Vyskytla se chyba!" << std::endl;

	return 0;
}

