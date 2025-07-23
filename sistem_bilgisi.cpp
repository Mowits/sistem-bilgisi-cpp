#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <memory>
#include <array>

using namespace std;

// Dosyadan ilk satırı okuyan yardımcı fonksiyon
string readFirstLine(const string& path) {
    ifstream file(path);
    string line;
    if (file.is_open() && getline(file, line)) {
        return line;
    }
    return "Bilinmiyor";
}

// Sistem komutunu çalıştırıp çıktısını alan fonksiyon
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        return "Komut çalıştırılamadı.";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    cout << "=== Sistem Bilgileri ===" << endl;

    // CPU Modeli
    string cpuModel = "Bilinmiyor";
    ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
        string line;
        while (getline(cpuinfo, line)) {
            if (line.find("model name") != string::npos) {
                size_t pos = line.find(":");
                if (pos != string::npos) {
                    cpuModel = line.substr(pos + 2);
                    break;
                }
            }
        }
        cpuinfo.close();
    }
    cout << "CPU Modeli: " << cpuModel << endl;

    // Çekirdek sayısı
    int cores = 0;
    ifstream cpuinfo2("/proc/cpuinfo");
    if (cpuinfo2.is_open()) {
        string line;
        while (getline(cpuinfo2, line)) {
            if (line.find("processor") != string::npos) {
                cores++;
            }
        }
        cpuinfo2.close();
    }
    cout << "Cekirdek Sayisi: " << cores << endl;

    // Toplam RAM
    string memTotal = readFirstLine("/proc/meminfo");
    cout << "Toplam RAM: ";
    if (memTotal.find("MemTotal:") != string::npos) {
        cout << memTotal.substr(memTotal.find(":") + 1);
    } else {
        cout << "Bilinmiyor";
    }
    cout << endl;

    // İşletim sistemi
    string osRelease = readFirstLine("/etc/os-release");
    cout << "Isletim Sistemi Bilgisi (ilk satir): " << osRelease << endl;

    // Ekran kartı bilgisi (lspci komutuyla)
    cout << "Ekran Kartı Bilgisi: " << endl;
    string gpuInfo = exec("lspci | grep -i 'vga\\|3d\\|2d'");
    if (gpuInfo.empty()) {
        cout << "Bilinmiyor veya lspci komutu bulunamadı." << endl;
    } else {
        cout << gpuInfo;
    }

    return 0;
}
