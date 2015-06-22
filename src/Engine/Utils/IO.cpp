
bool ParseGLSL(const std::string& filename, std::string& outFile) {
      std::ifstream fileStream(filename);

      if (fileStream.is_open()) {
            std::string line;
            while (getline(fileStream, line)) {
                  //Check if the file has a include directive
                  if (line.find("#") != std::string::npos) {
                        if (line.find("include") != std::string::npos) {
                              size_t begin = line.find("<") + 1;
                              size_t end = line.find(">") - begin;
                              std::string includeFilename = line.substr(begin, end);
                              std::string includeFile;
                              processShaderFile(includeFilename, includeFile);
                              outFile.append(includeFile);
                              continue;	//Dont add the include line...
                        }
                  }
                  outFile.append(line);
                  outFile.append("\n");
            }

            fileStream.close();
            return true;
      }

      LOG_ERROR("could not open " << filename);
      return false;
}
