#include <iostream>
#include <vector>

// 1) implementujte RAII obal nad souborem ze standardni knihovny C
class RAII_File
{
	public:
		// ???

		// 2) doplnte metodu Read_Line
		std::string Read_Line()
		{
			// ???
			return "";
		}

		// 3) doplnte metodu Write_Line
		void Write_Line(const std::string& line)
		{
			// ???
		}

		// ???
};

// 4) implementujte metodu, ktera vygeneruje obsah do nekolika souboru
void Generate_Files()
{
	// a) vytvorte vektor souboru
	// b) vlozte do nej nekolik instanci RAII_File souboru, do kterych budeme zapisovat
	// c) vygenerujte nejaky obsah, ktery do souboru budete vkladat;
	//    - kazdy soubor by mel mit ruzny obsah
	//    - pristupujte ke vsem souborum v cyklu nad vektorem
}

// 5) implementujte metodu, ktera overi, ze v souborech je skutecne ten obsah, ktery tam ma byt
bool Verify_Files()
{
	// a) vytvorte vektor souboru
	// b) vlozte do nej nekolik instanci RAII_File souboru, ze kterych budeme cist
	// c) overte, ze v souborech je to, co jste do nich v Generate_Files zapsali
	//    - vrate true, pokud ano, jinak vratte false

	return false;
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

