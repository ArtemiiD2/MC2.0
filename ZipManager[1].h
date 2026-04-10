#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include "minizip/zip.h"
#include "minizip/unzip.h"

namespace fs = std::filesystem;

class ZipManager {
public:


    bool createZipFromFolder(const std::string& folderPath, const std::string& zipPath)
    {
        fs::remove_all(folderPath);
    }
        // Create zip from folder (pack once)
    //bool createZipFromFolder(const std::string& folderPath, const std::string& zipPath) {
    //    // Check if source folder exists
    //    if (!fs::exists(folderPath)) {
    //        std::cout << "Error: Source folder '" << folderPath << "' does not exist!" << std::endl;
    //        return false;
    //    }

    //    zipFile zf = zipOpen64(zipPath.c_str(), APPEND_STATUS_CREATE);
    //    if (!zf) {
    //        std::cout << "Error: Cannot create zip file '" << zipPath << "'!" << std::endl;
    //        return false;
    //    }

    //    bool success = true;
    //    int fileCount = 0;

    //    try {
    //        for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
    //            if (entry.is_regular_file()) {
    //                std::string relativePath = fs::relative(entry.path(), folderPath).string();
    //                std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

    //                zip_fileinfo fileinfo = {};
    //                if (zipOpenNewFileInZip(zf, relativePath.c_str(), &fileinfo,
    //                    nullptr, 0, nullptr, 0, nullptr,
    //                    Z_DEFLATED, Z_BEST_COMPRESSION) == ZIP_OK) {

    //                    FILE* fin = fopen(entry.path().string().c_str(), "rb");
    //                    if (fin) {
    //                        char buffer[8192];
    //                        size_t read;
    //                        while ((read = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
    //                            zipWriteInFileInZip(zf, buffer, read);
    //                        }
    //                        fclose(fin);
    //                        fileCount++;
    //                    }
    //                    zipCloseFileInZip(zf);
    //                }
    //            }
    //        }
    //        std::cout << "Success: Packed " << fileCount << " files into '" << zipPath << "'" << std::endl;
    //    }
    //    catch (const std::exception& e) {
    //        std::cout << "Error: " << e.what() << std::endl;
    //        success = false;
    //    }

    //    zipClose(zf, nullptr);

    //    fs::remove_all(folderPath);
    //    //fs::remove_all(zipPath);

    //    return success;
    //}

    // Extract entire zip to folder
    bool extractZipToFolder(const std::string& zipPath, const std::string& outputFolder) {
        // Check if zip file exists
        if (!fs::exists(zipPath)) {
            std::cout << "Error: Zip file '" << zipPath << "' does not exist!" << std::endl;
            return false;
        }

        unzFile uf = unzOpen64(zipPath.c_str());
        if (!uf) {
            std::cout << "Error: Cannot open zip file '" << zipPath << "'!" << std::endl;
            return false;
        }

        // Create output folder
        fs::create_directories(outputFolder);

        bool success = true;
        int fileCount = 0;

        if (unzGoToFirstFile(uf) == UNZ_OK) {
            do {
                char filename[256];
                unz_file_info64 file_info;
                if (unzGetCurrentFileInfo64(uf, &file_info, filename,
                    sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK) {
                    std::cout << "Error: Cannot read file info from zip!" << std::endl;
                    success = false;
                    break;
                }

                std::string fullpath = outputFolder + "/" + filename;

                // Create subdirectories if needed
                fs::path filePath(fullpath);
                fs::create_directories(filePath.parent_path());

                if (unzOpenCurrentFile(uf) != UNZ_OK) {
                    std::cout << "Error: Cannot open file '" << filename << "' in zip!" << std::endl;
                    success = false;
                    break;
                }

                FILE* fout = fopen(fullpath.c_str(), "wb");
                if (fout) {
                    char buffer[8192];
                    int read;
                    while ((read = unzReadCurrentFile(uf, buffer, sizeof(buffer))) > 0) {
                        fwrite(buffer, 1, read, fout);
                    }
                    fclose(fout);
                    fileCount++;
                }
                else {
                    std::cout << "Error: Cannot create file '" << fullpath << "'!" << std::endl;
                    success = false;
                }

                unzCloseCurrentFile(uf);
            } while (success && unzGoToNextFile(uf) == UNZ_OK);
        }

        if (success) {
            std::cout << "Success: Extracted " << fileCount << " files to '" << outputFolder << "'" << std::endl;
        }

        unzClose(uf);
        fs::remove_all(zipPath);
        return success;
    }

    // Check if zip file exists
    bool zipExists(const std::string& zipPath) {
        return fs::exists(zipPath);
    }

    // Check if folder exists
    bool folderExists(const std::string& folderPath) {
        return fs::exists(folderPath) && fs::is_directory(folderPath);
    }


};