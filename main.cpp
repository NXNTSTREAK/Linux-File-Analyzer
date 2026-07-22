#include <iostream>
#include <string>
#include <optional>
#include <string_view>
#include <sys/stat.h>


//1. THE CONTAINER
struct FileMetadata{

    enum class Type{
        Regular,
        Directory,
        Symlink,
        Other
    };

    Type file_type;
    off_t file_size;
    mode_t permissions;
    time_t modification_time;

};

//THE ENGINE

class OSInterface{

    public:
        std::optional<FileMetadata> get_metadata(std::string_view file_path){

            std::string safe_path(file_path);  //Directly converts from string_view to string 
 
            struct stat kernel_data;

            int status = lstat(safe_path.c_str(), &kernel_data);

            if(status == -1){

                return std::nullopt;

            }

            struct FileMetadata fmd;

            fmd.file_size = kernel_data.st_size;
            fmd.modification_time = kernel_data.st_mtime;
            fmd.permissions = kernel_data.st_mode;

            if (S_ISREG(kernel_data.st_mode)) {
                fmd.file_type = FileMetadata::Type::Regular;
            } else if (S_ISDIR(kernel_data.st_mode)) {
                fmd.file_type = FileMetadata::Type::Directory;
            } else if (S_ISLNK(kernel_data.st_mode)) {
                fmd.file_type = FileMetadata::Type::Symlink;
            } else {
                fmd.file_type = FileMetadata::Type::Other;
            }

            return fmd;
        }

};

//THE FORMATTER

std::string format_permissions(mode_t perm){

    std:: string result = "";

    //FOR USER

    if( perm & S_IRUSR ){
        result += "r";
    }
    else{
        result += "-";
    }

    if (perm & S_IWUSR){
        result += "w";
    }
    else{
        result += "-";
    }

    if (perm & S_IXUSR){
        result += "x";
    }
    else{
        result += "-";
    }

    result += " ";

    //FOR GROUP
    if( perm & S_IRGRP ){
        result += "r";
    }
    else{
        result += "-";
    }

    if (perm & S_IWGRP){
        result += "w";
    }
    else{
        result += "-";
    }

    if (perm & S_IXGRP){
        result += "x";
    }
    else{
        result += "-";
    }
    
    result += " ";

    //FOR OTHERS
    if( perm & S_IROTH){
        result += "r";
    }
    else{
        result += "-";
    }

    if (perm & S_IWOTH){
        result += "w";
    }
    else{
        result += "-";
    }

    if (perm & S_IXOTH){
        result += "x";
    }
    else{
        result += "-";
    }

    return result;
}

std::string format_time(time_t raw_time) {
    char buffer[80];
    struct tm* timeinfo = localtime(&raw_time);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

int main(int argc, char* argv[]) {
    
    if(argc != 2){
        std::cerr << "Usage: ./analyzer <filepath> \n";
        return 1;
    }

    OSInterface os;

    std::optional<FileMetadata> result = os.get_metadata(argv[1]); 

    if (!result.has_value()) {
        std::cerr << "Couldn't find or access: " << argv[1] << "\n";
        return 1;
    }

    FileMetadata fmd = result.value();

    std::cout << "--- File Analysis ---\n";
    std::cout << "Target      : " << argv[1] << "\n";
    std::cout << "Size        : " << fmd.file_size << " bytes\n";
    std::cout << "Permissions : " << format_permissions(fmd.permissions) << "\n";
    std::cout << "Modified    : " << format_time(fmd.modification_time) << "\n";

    std::cout << "Type        : ";
    if (fmd.file_type == FileMetadata::Type::Directory) std::cout << "Directory\n";
    else if (fmd.file_type == FileMetadata::Type::Symlink) std::cout << "Symlink\n";
    else if (fmd.file_type == FileMetadata::Type::Regular) std::cout << "Regular File\n";
    else std::cout << "Other\n";

    return 0;
}