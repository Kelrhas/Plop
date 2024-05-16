
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char **argv)
{
	filesystem::path enginePath = filesystem::canonical(filesystem::current_path());
	if (argc > 1)
		enginePath = filesystem::canonical(argv[1]);
	else
		cout << "No path given, taking the current one :" << filesystem::current_path() << endl;

	filesystem::path cmakeFile = enginePath / "CMakeLists.txt";

	if (ifstream fileIn { cmakeFile })
	{
		string sFileContent { (istreambuf_iterator<char>(fileIn)), istreambuf_iterator<char>() };

		// first backup the file
		if (ofstream copy { cmakeFile / ".bak" })
		{
			copy.write(sFileContent.c_str(), sFileContent.size());
		}
		fileIn.close();

		// then remove old dynamic targets
		string_view svFile = sFileContent;
		size_t		idx	   = svFile.find("DYNAMIC_TARGETS");
		idx				   = svFile.find_first_of("\n", idx);

		sFileContent = (string)svFile.substr(0, idx);

		// and write the new content
		if (ofstream fileOut { cmakeFile, ios::out })
		{
			// fileOut << svGoodContent;
			fileOut << sFileContent << endl;

			filesystem::path parentPath = enginePath.parent_path();
			for (auto const &dir : filesystem::directory_iterator { parentPath })
			{
				if (dir.is_directory() && dir != enginePath)
				{
					if (filesystem::exists(dir.path() / "CMakeLists.txt"))
					{
						string sDir = filesystem::relative( dir.path(), enginePath).string();
						replace(sDir.begin(), sDir.end(), '\\', '/');
						string sEngineDir = enginePath.string();
						replace(sEngineDir.begin(), sEngineDir.end(), '\\', '/');

						cout << "Found " << sDir << endl;
						fileOut << "add_subdirectory(" << sDir << " " << sDir << "-build)" << endl;
					}
				}
			}

			fileOut.close();
		}
	}
	else
	{
		cerr << "No file found at:" << cmakeFile << endl;
	}
}