#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctime>
#include <cerrno>
#include <cstring>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

#define RGB(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"

// cols for file types (to be expanded) (to be put into a diff file for a bettr project structure)
std::string get_color(mode_t mode, const std::string& ext) {
    if (S_ISDIR(mode)) return BOLD RGB(52,152,219);
    if (S_ISLNK(mode)) return RGB(236,240,241);
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return RGB(46,204,113);
    if (ext == ".jpg" || ext == ".png" || ext == ".gif" || ext == ".jpeg" || ext == ".svg" || ext == ".bmp") return RGB(155,89,182);
    if (ext == ".mp3" || ext == ".wav" || ext == ".flac" || ext == ".ogg") return RGB(231,76,60);
    if (ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".mov") return RGB(230,126,34);
    if (ext == ".pdf" || ext == ".doc" || ext == ".docx" || ext == ".odt") return RGB(192,57,43);
    if (ext == ".zip" || ext == ".tar" || ext == ".gz" || ext == ".rar" || ext == ".7z") return RGB(241,196,15);
    if (ext == ".txt" || ext == ".md" || ext == ".log" || ext == ".rtf") return RGB(236,240,241);
    if (ext == ".json" || ext == ".yaml" || ext == ".yml" || ext == ".xml" || ext == ".toml" || ext == ".ini") return RGB(149,165,166);
    if (ext == ".csv" || ext == ".xlsx" || ext == ".xls" || ext == ".tsv") return RGB(39,174,96);
    if (ext == ".sql" || ext == ".db" || ext == ".sqlite") return RGB(52,73,94);
    if (ext == ".dockerfile" || ext == ".yml" ) return RGB(41,128,185);
    if (ext == ".cpp" || ext == ".c" || ext == ".hpp" || ext == ".h" || ext == ".cc") return RGB(52,152,219);
    if (ext == ".py" || ext == ".pyc") return RGB(241,196,15);
    if (ext == ".js" || ext == ".ts" || ext == ".jsx" || ext == ".tsx") return RGB(241,196,15);
    if (ext == ".html" || ext == ".htm") return RGB(230,126,34);
    if (ext == ".css" || ext == ".scss" || ext == ".sass") return RGB(155,89,182);
    if (ext == ".rs") return RGB(192,57,43);
    if (ext == ".go") return RGB(52,152,219);
    if (ext == ".java" || ext == ".class" || ext == ".jar") return RGB(192,57,43);
    if (ext == ".php") return RGB(155,89,182);
    if (ext == ".rb") return RGB(192,57,43);
    if (ext == ".sh" || ext == ".bash") return RGB(46,204,113);
    if (ext == ".pl" || ext == ".pm") return RGB(142,68,173);
    if (ext == ".lua") return RGB(52,73,94);
    if (ext == ".kt" || ext == ".kts") return RGB(230,126,34);
    if (ext == ".swift") return RGB(231,76,60);
    return RGB(189,195,199);
}

// color list for lineras
const std::string GREY_GRADIENT[] = {
    RGB(245,245,245),
    RGB(236,240,241),
    RGB(220,220,220),
    RGB(189,195,199),
    RGB(169,169,169),
    RGB(149,165,166),
    RGB(128,128,128),
    RGB(105,105,105),
    RGB(80,80,80),
    RGB(52,73,94)
};
const int NUM_GREYS = sizeof(GREY_GRADIENT) / sizeof(GREY_GRADIENT[0]);

// types
std::string get_icon(mode_t mode, const std::string& ext) {
    if (S_ISDIR(mode)) return "📁 ";
    if (S_ISLNK(mode)) return "🔗 ";
    if (ext == ".txt" || ext == ".md" || ext == ".log" || ext == ".rtf") return "📄 ";
    if (ext == ".jpg" || ext == ".png" || ext == ".gif" || ext == ".jpeg" || ext == ".svg" || ext == ".bmp") return "🖼️ ";
    if (ext == ".mp3" || ext == ".wav" || ext == ".flac" || ext == ".ogg") return "🎵 ";
    if (ext == ".mp4" || ext == ".avi" || ext == ".mkv" || ext == ".mov") return "🎥 ";
    if (ext == ".pdf" || ext == ".doc" || ext == ".docx" || ext == ".odt") return "📕 ";
    if (ext == ".zip" || ext == ".tar" || ext == ".gz" || ext == ".rar" || ext == ".7z") return "🗜️ ";
    if (ext == ".sh" || ext == ".py" || ext == ".cpp" || ext == ".c" || ext == ".hpp" || ext == ".h" || ext == ".cc" || ext == ".bash") return "📜 ";
    if (ext == ".js" || ext == ".ts" || ext == ".jsx" || ext == ".tsx") return "📜 ";
    if (ext == ".html" || ext == ".htm") return "🌐 ";
    if (ext == ".css" || ext == ".scss" || ext == ".sass") return "🎨 ";
    if (ext == ".json" || ext == ".yaml" || ext == ".yml" || ext == ".xml" || ext == ".toml" || ext == ".ini") return "📋 ";
    if (ext == ".csv" || ext == ".xlsx" || ext == ".xls" || ext == ".tsv") return "📊 ";
    if (ext == ".sql" || ext == ".db" || ext == ".sqlite") return "🗄️ ";
    if (ext == ".dockerfile") return "🐳 ";
    if (ext == ".rs") return "🦀 ";
    if (ext == ".go") return "🐹 ";
    if (ext == ".java" || ext == ".class" || ext == ".jar") return "☕ ";
    if (ext == ".php") return "🐘 ";
    if (ext == ".rb") return "💎 ";
    if (ext == ".pl" || ext == ".pm") return "🦪 ";
    if (ext == ".lua") return "🌙 ";
    if (ext == ".kt" || ext == ".kts") return "🤖 ";
    if (ext == ".swift") return "🐦 ";
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return "⚙️ ";
    return "📄 ";
}

// perms and whatnot
std::string get_permissions(mode_t mode) {
    std::string perm(10, '-');
    perm[0] = (S_ISDIR(mode)) ? 'd' : (S_ISLNK(mode) ? 'l' : (S_ISFIFO(mode) ? 'p' : (S_ISCHR(mode) ? 'c' : (S_ISBLK(mode) ? 'b' : (S_ISSOCK(mode) ? 's' : '-')))));
    perm[1] = (mode & S_IRUSR) ? 'r' : '-';
    perm[2] = (mode & S_IWUSR) ? 'w' : '-';
    perm[3] = (mode & S_IXUSR) ? 'x' : '-';
    perm[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm[5] = (mode & S_IWGRP) ? 'w' : '-';
    erm[6] = (mode & S_IXGRP) ? 'x' : '-';
    perm[7] = (mode & S_IROTH) ? 'r' : '-';
    perm[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm[9] = (mode & S_IXOTH) ? 'x' : '-';
    return perm;
}

std::string get_owner(uid_t uid) {
    struct passwd* pw = getpwuid(uid);
    return pw ? pw->pw_name : std::to_string(uid);
}

std::string get_group(gid_t gid) {
    struct group* gr = getgrgid(gid);
    return gr ? gr->gr_name : std::to_string(gid);
}

std::string human_size(off_t size) {
    if (size < 0) return "--";
    const char* units[] = {"B", "K", "M", "G", "T", "P", "E"};
    int i = 0;
    double dsize = static_cast<double>(size);
    while (dsize >= 1024 && i < 6) {
        dsize /= 1024;
        i++;
    }
    std::ostringstream oss;
    if (dsize < 10) {
        oss << std::fixed << std::setprecision(1) << dsize;
    } else {
        oss << std::fixed << std::setprecision(0) << dsize;
    }
    oss << units[i];
    return oss.str();
}

std::string get_mtime(time_t mtime) {
    time_t now = time(nullptr);
    double seconds = difftime(now, mtime);
    char buf[20];
    std::tm* tm = localtime(&mtime);
    if (tm == nullptr) return "???? ?? ??:??";  // issue fallback
    if (std::abs(seconds) < 15552000) {
        std::strftime(buf, sizeof(buf), "%b %d %H:%M", tm);
    } else {
        std::strftime(buf, sizeof(buf), "%b %d  %Y", tm);
    }
    return buf;
}

// Print help message
void print_help(const std::string& prog_name) {
    std::cout << BOLD << "mls" << RESET << " - Modern, colorful ls alternative\n\n";
    std::cout << "Usage: " << prog_name << " [OPTIONS] [PATH]\n\n";
    std::cout << "Options (use +short or ++long form):\n";
    std::cout << "  +a, ++all         Show hidden files (starting with .)\n";
    std::cout << "  +l, ++long        Long format (details like permissions, size, time)\n";
    std::cout << "  +t, ++tree        Tree view (directories only)\n";
    std::cout << "  +ta, ++treeall    Tree view (with files, like Windows TREE /F)\n";
    std::cout << "  +s, ++size        Show file sizes in non-long modes (e.g., short or tree)\n";
    std::cout << "  +d, ++dirs-first  List directories before files\n";
    std::cout << "  +sort=TYPE        Sort by: name (default), size, time\n";
    std::cout << "  +r, ++reverse     Reverse sort order\n";
    std::cout << "  +h, ++help        Show this help message\n\n";
    std::cout << "Advanced Features:\n";
    std::cout << "- Type-specific icons and RGB colors\n";
    std::cout << "- Smooth multi-shade grey gradients in tree branches\n";
    std::cout << "- Robust error handling with fallbacks\n";
    std::cout << "- Responsive layouts with dynamic alignment\n";
    std::cout << "Version: 1.1 (2025)\n";
}

// tree
void print_tree(const std::filesystem::path& p, bool show_hidden, bool show_files_in_tree, bool show_sizes, std::string indent = "", bool is_last = true, int depth = 0) {
    struct stat st;
    if (lstat(p.c_str(), &st) != 0) {
        std::cerr << RGB(192,57,43) << "Error accessing " << p << ": " << strerror(errno) << RESET << std::endl;
        return;
    }

    std::string name = p.filename().string();
    if (name.empty()) name = p.string();

    if (!indent.empty()) {
        int grad_index = depth % NUM_GREYS;
        if (depth / NUM_GREYS % 2 == 1) grad_index = NUM_GREYS - 1 - grad_index;
        std::string branch_color = GREY_GRADIENT[grad_index];
        std::cout << branch_color << indent << (is_last ? "└── " : "├── ") << RESET;
    }

    std::string ext = p.extension().string();
    std::string color = get_color(st.st_mode, ext);
    std::string icon = get_icon(st.st_mode, ext);
    std::cout << color << icon << name;
    if (show_sizes) {
        std::cout << " (" << human_size(st.st_size) << ")";
    }
    std::cout << RESET << std::endl;

    if (!S_ISDIR(st.st_mode)) return;

    std::vector<std::filesystem::path> paths;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(p, std::filesystem::directory_options::skip_permission_denied)) {
            std::string fn = entry.path().filename().string();
            if (!show_hidden && !fn.empty() && fn[0] == '.') continue;
            if (!show_files_in_tree && !std::filesystem::is_directory(entry)) continue;
            paths.push_back(entry.path());
        }
        std::sort(paths.begin(), paths.end());
    } catch (const std::exception& e) {
        std::cerr << RGB(192,57,43) << "Error reading " << p << ": " << e.what() << RESET << std::endl;
        return;
    }

    for (size_t i = 0; i < paths.size(); ++i) {
        bool last = (i == paths.size() - 1);
        print_tree(paths[i], show_hidden, show_files_in_tree, show_sizes, indent + (is_last ? "    " : "│   "), last, depth + 1);
    }
}

int main(int argc, char* argv[]) {
    std::string path = ".";
    bool long_format = false;
    bool show_hidden = false;
    bool tree_mode = false;
    bool tree_all = false;
    bool show_sizes = false;
    bool dirs_first = false;
    bool reverse_sort = false;
    std::string sort_type = "name";
    bool show_help = false;

    // custom arg parser (im sick of defaults)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '+') {
            if (arg == "+a" || arg == "++all") show_hidden = true;
            else if (arg == "+l" || arg == "++long") long_format = true;
            else if (arg == "+t" || arg == "++tree") tree_mode = true;
            else if (arg == "+ta" || arg == "++treeall") { tree_mode = true; tree_all = true; }
            else if (arg == "+s" || arg == "++size") show_sizes = true;
            else if (arg == "+d" || arg == "++dirs-first") dirs_first = true;
            else if (arg == "+r" || arg == "++reverse") reverse_sort = true;
            else if (arg == "+h" || arg == "++help") show_help = true;
            else if (arg.find("+sort=") == 0 || arg.find("++sort=") == 0) {
                size_t pos = arg.find('=') + 1;
                sort_type = arg.substr(pos);
                if (sort_type != "name" && sort_type != "size" && sort_type != "time") {
                    std::cerr << RGB(192,57,43) << "Invalid sort: " << sort_type << ". Valid: name, size, time." << RESET << std::endl;
                    return 1;
                }
            } else {
                std::cerr << RGB(192,57,43) << "Unknown option: " << arg << RESET << std::endl;
                print_help(argv[0]);
                return 1;
            }
        } else {
            path = arg;
        }
    }

    if (show_help) {
        print_help(argv[0]);
        return 0;
    }

    // path validation
    struct stat path_st;
    if (lstat(path.c_str(), &path_st) != 0) {
        std::cerr << RGB(192,57,43) << "Invalid path " << path << ": " << strerror(errno) << RESET << std::endl;
        return 1;
    }

    std::vector<std::filesystem::directory_entry> entries;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied)) {
            std::string filename = entry.path().filename().string();
            if (!show_hidden && !filename.empty() && filename[0] == '.') continue;
            entries.push_back(entry);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << RGB(192,57,43) << "Directory error: " << e.what() << RESET << std::endl;
        return 1;
    }

    auto sorter = [&](const auto& a, const auto& b) -> bool {
        struct stat sa, sb;
        if (lstat(a.path().c_str(), &sa) != 0 || lstat(b.path().c_str(), &sb) != 0) return false;

        if (dirs_first) {
            bool da = S_ISDIR(sa.st_mode), db = S_ISDIR(sb.st_mode);
            if (da != db) return da > db;
        }

        if (sort_type == "size") {
            if (sa.st_size != sb.st_size) return sa.st_size > sb.st_size;
        } else if (sort_type == "time") {
            if (sa.st_mtime != sb.st_mtime) return sa.st_mtime > sb.st_mtime;
        }
        return a.path().filename() < b.path().filename();
    };

    std::sort(entries.begin(), entries.end(), sorter);
    if (reverse_sort) std::reverse(entries.begin(), entries.end());

    if (tree_mode) {
        print_tree(path, show_hidden, tree_all, show_sizes);
    } else if (long_format) {
        // as bellow
        blkcnt_t total_blocks = 0;
        for (const auto& entry : entries) {
            struct stat st;
            if (lstat(entry.path().c_str(), &st) == 0) total_blocks += st.st_blocks;
        }
        std::cout << "total " << (total_blocks / 2) << std::endl;  // calc for sizes

        // max widths
        size_t max_nlink = 1, max_owner = 1, max_group = 1, max_size = 1;
        for (const auto& entry : entries) {
            struct stat st;
            if (lstat(entry.path().c_str(), &st) != 0) continue;
            max_nlink = std::max(max_nlink, std::to_string(st.st_nlink).length());
            max_owner = std::max(max_owner, get_owner(st.st_uid).length());
            max_group = std::max(max_group, get_group(st.st_gid).length());
            max_size = std::max(max_size, human_size(st.st_size).length());
        }

        for (const auto& entry : entries) {
            struct stat st;
            if (lstat(entry.path().c_str(), &st) != 0) {
                std::cerr << RGB(192,57,43) << "Stat error: " << entry.path() << ": " << strerror(errno) << RESET << std::endl;
                continue;
            }
            std::string ext = entry.path().extension().string();
            std::string color = get_color(st.st_mode, ext);
            std::string icon = get_icon(st.st_mode, ext);
            std::string filename = entry.path().filename().string();

            std::cout << get_permissions(st.st_mode) << " "
                      << std::right << std::setw(max_nlink) << st.st_nlink << " "
                      << std::left << std::setw(max_owner + 1) << get_owner(st.st_uid) << " "
                      << std::setw(max_group + 1) << get_group(st.st_gid) << " "
                      << std::right << std::setw(max_size) << human_size(st.st_size) << " "
                      << std::left << std::setw(12) << get_mtime(st.st_mtime) << " "
                      << color << icon << filename;

            if (S_ISLNK(st.st_mode)) {
                try {
                    std::cout << " -> " << std::filesystem::read_symlink(entry.path()).string();
                } catch (const std::exception&) {
                    std::cout << " -> " << RGB(192,57,43) << "[broken]" << RESET;
                }
            }
            std::cout << RESET << std::endl;
        }
    } else {
        // short mode
        int max_len = 0;
        for (const auto& entry : entries) {
            struct stat st;
            if (lstat(entry.path().c_str(), &st) != 0) continue;
            std::string ext = entry.path().extension().string();
            std::string icon = get_icon(st.st_mode, ext);
            std::string filename = entry.path().filename().string();
            int len = icon.length() + filename.length();
            if (show_sizes) len += human_size(st.st_size).length() + 3;
            if (len > max_len) max_len = len;
        }
        max_len += 2;

        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0) {
            w.ws_col = 80;  // Fallback
        }
        int cols = w.ws_col / max_len;
        if (cols < 1) cols = 1;

        for (size_t i = 0; i < entries.size(); ++i) {
            auto& entry = entries[i];
            struct stat st;
            if (lstat(entry.path().c_str(), &st) != 0) continue;
            std::string ext = entry.path().extension().string();
            std::string color = get_color(st.st_mode, ext);
            std::string icon = get_icon(st.st_mode, ext);
            std::string filename = entry.path().filename().string();
            std::string size_str = show_sizes ? " (" + human_size(st.st_size) + ")" : "";
            int item_len = icon.length() + filename.length() + size_str.length();

            std::cout << color << icon << filename << size_str << RESET << std::string(max_len - item_len, ' ');
            if ((i + 1) % cols == 0 || i == entries.size() - 1) std::cout << std::endl;
        }
    }

    return 0;
}
