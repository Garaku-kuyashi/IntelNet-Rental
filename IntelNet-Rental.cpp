#include <iostream>
#include <algorithm>
#include <cctype>     
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <limits>
using namespace std;

const int MAX_USERS = 100;
const int MAX_SERVERS = 50;
const int MAX_HISTORY = 100;
const int MAX_REQUESTS = 100;

struct User {
    string username;
    string password;
    string nama;
    string status;
    string server; 
};

struct Server {
    string id;
    string nama;
    int kapasitas;
    int pengguna_sekarang;
};

struct History {
    string waktu;
    string aksi;
};

struct ChangeRequest {
    string username;
    string nama;
    string password;
    string server;
    string status;
};

User users[MAX_USERS];
Server servers[MAX_SERVERS];
History histories[MAX_HISTORY];
ChangeRequest changeRequests[MAX_REQUESTS];

int userCount = 0;
int serverCount = 0;
int historyCount = 0;
int requestCount = 0;

User currentUser;

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string getCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buffer);
}

void addHistory(const string& action) {
    if (historyCount < MAX_HISTORY) {
        histories[historyCount++] = {getCurrentTime(), action};
    }
}

void loadData() {
    ifstream file("IntelNet Rental.csv");
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find(',');
        string type = line.substr(0, pos);
        string data = line.substr(pos + 1);

        if (type == "user") {
            string fields[5];
            size_t start = 0, idx = 0;
            while ((pos = data.find(',', start)) != string::npos && idx < 5) {
                fields[idx++] = data.substr(start, pos - start);
                start = pos + 1;
            }
            fields[idx++] = data.substr(start);
            if (idx == 5 && userCount < MAX_USERS) {
                users[userCount++] = {fields[0], fields[1], fields[2], fields[3], fields[4]};
            }
        } else if (type == "server") {
            string fields[4];
            size_t start = 0, idx = 0;
            while ((pos = data.find(',', start)) != string::npos && idx < 4) {
                fields[idx++] = data.substr(start, pos - start);
                start = pos + 1;
            }
            fields[idx++] = data.substr(start);
            if (idx == 4 && serverCount < MAX_SERVERS) {
                servers[serverCount++] = {fields[0], fields[1], stoi(fields[2]), stoi(fields[3])};
            }
        } else if (type == "history") {
            size_t split_pos = data.find(" - ");
            if (split_pos != string::npos && historyCount < MAX_HISTORY) {
                histories[historyCount++] = {data.substr(0, split_pos), data.substr(split_pos + 3)};
            }
        } else if (type == "change_request") {
            string fields[5];
            size_t start = 0, idx = 0;
            while ((pos = data.find(',', start)) != string::npos && idx < 5) {
                fields[idx++] = data.substr(start, pos - start);
                start = pos + 1;
            }
            fields[idx++] = data.substr(start);
            if (idx >= 5 && requestCount < MAX_REQUESTS) {
                changeRequests[requestCount++] = {fields[0], fields[1], fields[2], fields[3], fields[4]};
            }
        }
    }
    file.close();
}

void saveData() {
    ofstream file("IntelNet Rental.csv");
    if (!file.is_open()) {
        cout << "Gagal membuka file CSV!\n";
        return;
    }

    file << "tipe,data\n";

    file << "# Tipe: user\n# Format: username,password,nama,status,server\n";
    for (int i = 0; i < userCount; ++i) {
        file << "user," 
             << users[i].username << "," 
             << users[i].password << "," 
             << users[i].nama << "," 
             << users[i].status << "," 
             << users[i].server << "\n";
    }

    file << "\n# Tipe: server\n# Format: id,nama,kapasitas,pengguna_sekarang\n";
    for (int i = 0; i < serverCount; ++i) {
        file << "server," 
             << servers[i].id << "," 
             << servers[i].nama << "," 
             << servers[i].kapasitas << "," 
             << servers[i].pengguna_sekarang << "\n";
    }

    file << "\n# Tipe: history\n# Format: waktu - aksi\n";
    for (int i = 0; i < historyCount; ++i) {
        file << "history," << histories[i].waktu << " - " << histories[i].aksi << "\n";
    }

    file << "\n# Tipe: change_request\n# Format: username,nama,password,server,status\n";
    for (int i = 0; i < requestCount; ++i) {
        file << "change_request," 
             << changeRequests[i].username << "," 
             << changeRequests[i].nama << "," 
             << changeRequests[i].password << "," 
             << changeRequests[i].server << "," 
             << changeRequests[i].status << "\n";
    }

    file.close();
}

bool login() {
    system("cls");
    string username, password;
    cout << "=== LOGIN ===\n";
    cout << "Username: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);

    for (int i = 0; i < userCount; ++i) {
        if (users[i].username == username && users[i].password == password) {
            currentUser = users[i];
            return true;
        }
    }
    cout << "Login gagal. Username atau password salah.\n";
    cout << "tekan enter untuk kembali";
    cin.get();
    return false;
}

void registerUser() {
    system("cls");
    string username, password, nama;
    cout << "=== REGISTRASI USER BARU ===\n";
    cout << "Username: ";
    getline(cin, username);

    for (int i = 0; i < userCount; ++i) {
        if (users[i].username == username) {
            cout << "Username sudah digunakan!\n";
            cout << "tekan enter untuk kembali";
            cin.get();
            return;
        }
    }

    cout << "Password: ";
    getline(cin, password);
    cout << "Nama Lengkap: ";
    getline(cin, nama);

    users[userCount++] = {username, password, nama, "inactive", "-"};
    cout << "Registrasi berhasil! Silakan ajukan server via Request Server.\n";
    saveData();
}

// void logout() {
//     system("cls");
//     if (currentUser.username.empty()) {
//         cout << "Tidak ada user yang login!\n";
//         return;
//     }
//     currentUser = User();
//     cout << "Logout berhasil!\n";
// }

//=================================================admin=====================================
void lihatrequest() {
    system("cls");
    cout << "\n=== PERMINTAAN PEMBUATAN AKUN ===\n";
    bool found = false;
    for (int i = 0; i < requestCount; ++i) {
        if (changeRequests[i].status == "pending" &&
            changeRequests[i].nama == "" &&
            changeRequests[i].password == "" &&
            !changeRequests[i].server.empty()) {
            cout << "Username: " << changeRequests[i].username << endl;
            cout << "Server Pilihan: " << changeRequests[i].server << endl;
            cout << "------------------------\n";
            found = true; 
        }
    }
    cout << "" << endl;
    cout << "tekan enter untuk kembali";
    cin.get();
    if (!found) cout << "Tidak ada permintaan pembuatan akun.\n";
}

void setujuakun() {
    system("cls");
    string username;
    string loop;

    do
    {
        cout << "Masukkan username untuk disetujui: ";
        getline(cin, username);

        int reqIdx = -1;
        for (int i = 0; i < requestCount; ++i) {
            if (changeRequests[i].username == username && changeRequests[i].status == "pending") {
                reqIdx = i;
                break;
            }
        }

        if (reqIdx == -1) {
            cout << "Permintaan tidak ditemukan atau sudah diproses.\n";
            cin.get();
            return;
        }

        string server = changeRequests[reqIdx].server;
        int srvIdx = -1;
        for (int i = 0; i < serverCount; ++i) {
            if (servers[i].nama == server && servers[i].pengguna_sekarang < servers[i].kapasitas) {
                srvIdx = i;
                break;
            }
        }

        if (srvIdx == -1) {
            cout << "Server tidak tersedia atau penuh.\n";
            cin.get();
            return;
        }

        for (int i = 0; i < userCount; ++i) {
            if (users[i].username == username) {
                users[i].server = server;
                users[i].status = "active";
                servers[srvIdx].pengguna_sekarang++;
                break;
            }
        }

        for (int i = reqIdx; i < requestCount - 1; ++i)
            changeRequests[i] = changeRequests[i + 1];
        requestCount--;

        addHistory(username + " Memesan akun dengan server yaitu:" + server);
        cout << "Akun berhasil diaktifkan dengan server: " << server << "\n";
        saveData();
        cout << "" << endl;
        cout << "Apakah ingin setujui yang lain? (y/n): ";
        cin >> loop;
        clearInputBuffer();
    } while (loop == "y");
    

}

void lihatubah() {
    system("cls");
    cout << "\n=== PERMINTAAN PERUBAHAN AKUN ===\n";
    bool found = false;
    for (int i = 0; i < requestCount; ++i) {
        if (changeRequests[i].status == "pending" &&
            (!changeRequests[i].nama.empty() || 
             !changeRequests[i].password.empty() ||
             !changeRequests[i].server.empty())) {
            cout << "Username: " << changeRequests[i].username << endl;
            cout << "Nama Baru: " << (changeRequests[i].nama.empty() ? "-" : changeRequests[i].nama) << endl;
            cout << "Server Baru: " << (changeRequests[i].server.empty() ? "-" : changeRequests[i].server) << endl;
            cout << "------------------------\n";
            found = true;   
        }
        cout << "" << endl;
        cout << "tekan enter untuk kembali";
        cin.get();
    }
    if (!found) cout << "Tidak ada permintaan perubahan akun.\n";
    cin.get();
}

void setujuubah() {
    system("cls");
    string username;
    string loop;
    do
    {
        cout << "Masukkan username untuk disetujui perubahannya: ";
        getline(cin, username);

        int reqIdx = -1;
        for (int i = 0; i < requestCount; ++i) {
            if (changeRequests[i].username == username && changeRequests[i].status == "pending") {
                reqIdx = i;
                break;
            }
        }

        if (reqIdx == -1) {
            cout << "Permintaan tidak ditemukan atau sudah diproses.\n";
            cin.get();
            return;
        }

        int userIndex = -1;
        for (int i = 0; i < userCount; ++i) {
            if (users[i].username == username) {
                userIndex = i;
                break;
            }
        }

        if (userIndex == -1) {
            cout << "User tidak ditemukan di sistem aktif.\n";
            cin.get();
            return;
        }

        if (!changeRequests[reqIdx].nama.empty())
            users[userIndex].nama = changeRequests[reqIdx].nama;

        if (!changeRequests[reqIdx].password.empty())
            users[userIndex].password = changeRequests[reqIdx].password;

        if (!changeRequests[reqIdx].server.empty()) {
            string oldServer = users[userIndex].server;
            string newServer = changeRequests[reqIdx].server;

            for (int i = 0; i < serverCount; ++i) {
                if (servers[i].nama == oldServer) {
                    servers[i].pengguna_sekarang--;
                    break;
                }
            }

            bool serverFound = false;
            for (int i = 0; i < serverCount; ++i) {
                if (servers[i].nama == newServer && servers[i].pengguna_sekarang < servers[i].kapasitas) {
                    servers[i].pengguna_sekarang++;
                    users[userIndex].server = newServer;
                    serverFound = true;
                    break;
                }
            }

            if (!serverFound && !newServer.empty()) {
                cout << "Server baru tidak tersedia atau penuh. Server tetap tidak berubah.\n";
            }
        }

        for (int i = reqIdx; i < requestCount - 1; ++i)
            changeRequests[i] = changeRequests[i + 1];
        requestCount--;
        cout << "Perubahan akun berhasil disetujui.\n";
        saveData();
        cout << "" << endl;
        cout << "Apakah ingin setujui yang lain? (y/n): ";
        cin >> loop;
        clearInputBuffer();
    } while (loop == "y");
    
}

void tambah() {
    system ("cls");
    string loop;

    do {
        if (serverCount >= MAX_SERVERS) {
            cout << "Batas server maksimal tercapai.\n";
            break;
        }

        Server s;
        s.id = to_string(serverCount + 1); 
        cout << "Nama Server: ";
        getline(cin, s.nama);
        int kapasitas;
        while (true) {
            cout << "Kapasitas: ";
            string input;
            cin >> input;
            clearInputBuffer();

            try {
                kapasitas = stoi(input);
                if (kapasitas > 0) break;
                else cout << "Kapasitas harus lebih besar dari nol.\n";
            } catch (...) {
                cout << "Harus berupa angka!\n";
            }
        }

        s.kapasitas = kapasitas;
        s.pengguna_sekarang = 0;
        servers[serverCount++] = s;
        cout << "Server berhasil ditambahkan.\n";

        saveData();

        cout << "Apakah ingin tambah lagi? (y/n): ";
        cin >> loop;
        clearInputBuffer();

    } while (loop == "y");
}

void lihat() {;
    system("cls");
    cout << "+----+--------------+------------+-----------------+" << endl;
    cout << "| ID | Nama Server  | Kapasitas  | Jumlah Pengguna |" << endl;
    cout << "+----+--------------+------------+-----------------+" << endl;
    for (int i = 0; i < serverCount; ++i)
    {
        cout << "| " << setw(2) << left << servers[i].id << " | ";
        cout << setw(12) << left << servers[i].nama.substr(0, 12) << " | ";
        cout << setw(10) << left << servers[i].kapasitas << " | ";
        cout << setw(15) << left << servers[i].pengguna_sekarang << " | \n"; 
    }
    cout << "+----+--------------+------------+-----------------+ \n";
    cout << "" << endl;
    cout << "tekan enter untuk kembali";
    cin.get();
}


void cariServer(const string& keyword) {
    if (keyword.empty()) {
        cout << "Kata kunci kosong. Silakan masukkan huruf atau nama server.\n";
        return;
    }

    string keywordLower = keyword;
    transform(keywordLower.begin(), keywordLower.end(), keywordLower.begin(), ::tolower);

    bool found = false;
    cout << "\n=== HASIL PENCARIAN ===\n";
    for (int i = 0; i < serverCount; ++i) {
        string namaLower = servers[i].nama;
        transform(namaLower.begin(), namaLower.end(), namaLower.begin(), ::tolower);

        if (namaLower.find(keywordLower) != string::npos) {
            cout << servers[i].nama << " (" 
                 << servers[i].pengguna_sekarang << "/" 
                 << servers[i].kapasitas << ")\n";
            found = true;
        }
    }

    if (!found) {
        cout << "Tidak ada server ditemukan dengan kata kunci '" << keyword << "'.\n";
    }
}

void pengguna_terbanyak() {
    system("cls");

    if (serverCount == 0) {
        cout << "Tidak ada server tersedia.\n";
        cout << "tekan enter...";
        cin.get();
        clearInputBuffer();
        return;
    }

    Server temp[MAX_SERVERS];
    for (int i = 0; i < serverCount; ++i)
        temp[i] = servers[i];

    for (int i = 0; i < serverCount - 1; ++i)
        for (int j = i + 1; j < serverCount; ++j)
            if (temp[i].pengguna_sekarang < temp[j].pengguna_sekarang)
                swap(temp[i], temp[j]);

    cout << "+----+--------------+------------+-----------------+" << endl;
    cout << "| ID | Nama Server  | Kapasitas  | Jumlah Pengguna |" << endl;
    cout << "+====+==============+============+=================+" << endl;

    for (int i = 0; i < serverCount; ++i) {
        cout << "| " << setw(2) << left << temp[i].id << " | ";
        cout << setw(12) << left << temp[i].nama.substr(0, 12) << " | ";
        cout << setw(9) << left << temp[i].kapasitas << " | ";
        cout << setw(16) << left << temp[i].pengguna_sekarang << " | " << endl;
    }

    cout << "+----+--------------+------------+-----------------+\n\n";

    cout << "Apakah ingin mencari server? (y/n): ";
    string pilih;
    getline(cin, pilih);

    if (pilih == "y") {
        string keyword;
        cout << "Masukkan kata kunci pencarian: ";
        getline(cin, keyword);
        cariServer(keyword);
    }

    cout << "\ntekan enter untuk kembali...";
    cin.get();
    clearInputBuffer();
}

void editServer(string name) { 
    string loop;

    do
    {
        system("cls");
        cout << "+----+--------------+------------+-----------------+" << endl;
        cout << "| ID | Nama Server  | Kapasitas  | Jumlah Pengguna |" << endl;
        cout << "+====+==============+============+=================+" << endl;
        for (int i = 0; i < serverCount; ++i) {
            cout << "| " << setw(2) << left << servers[i].id << " | ";
            cout << setw(12) << left << servers[i].nama.substr(0, 12) << " | ";
            cout << setw(9) << left << servers[i].kapasitas << " | ";
            cout << setw(16) << left << servers[i].pengguna_sekarang << " | " << endl;
        }
        cout << "+----+--------------+------------+-----------------+\n\n";
        cout << "Masukkan nama server yang ingin diedit: ";
        getline(cin, name);
        int idx = -1;
        for (int i = 0; i < serverCount; ++i) {
            if (servers[i].nama == name) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            cout << "Server tidak ditemukan.\n";
            cout << "tekan enter untuk kembali...";
            cin.get();
            clearInputBuffer();
            return;
        }

        Server* s = &servers[idx]; 

        cout << "Nama Baru (kosongkan jika tidak ingin mengubah): ";
        string newName;
        getline(cin, newName);
        if (!newName.empty()) {
            s->nama = newName; 
        }

        bool angka = false;
        while (!angka) {
            cout << "Kapasitas Baru (kosongkan jika tidak ingin mengubah): ";
            string capStr;
            getline(cin, capStr);

            if (capStr.empty()) {
                angka = true;
                continue;
            }

            try {
                int newCap = stoi(capStr);
                if (newCap > s->pengguna_sekarang) { // 🔥 Pakai s->
                    s->kapasitas = newCap;         // 🔥
                    angka = true;
                    cout << "Kapasitas berhasil diubah.\n";
                } else {
                    cout << "Kapasitas harus lebih besar dari jumlah pengguna sekarang (" 
                        << s->pengguna_sekarang << ").\n"; // 🔥
            }
            } catch (...) {
                cout << "Harus berupa angka.\n";
            }
        }
        cout << "Server berhasil diubah.\n";
        saveData();
        cout << "\nApakah ingin ubah lagi? (y/n): ";
        cin >> loop;
        clearInputBuffer();
    } while (loop == "y");
    
}

void hapuserver() {
    system("cls");
    string name;
    string loop;

    do
    {
        cout << "+----+--------------+------------+-----------------+" << endl;
        cout << "| ID | Nama Server  | Kapasitas  | Jumlah Pengguna |" << endl;
        cout << "+----+--------------+------------+-----------------+" << endl;
        for (int i = 0; i < serverCount; ++i)
        {
            cout << "| " << setw(2) << left << servers[i].id << " | ";
            cout << setw(12) << left << servers[i].nama.substr(0, 12) << " | ";
            cout << setw(9) << left << servers[i].kapasitas << " | ";
            cout << setw(16) << left << servers[i].pengguna_sekarang << " | \n";
            
        }
        cout << "+----+--------------+------------+-----------------+ \n" << endl;
        cout << "" << endl;
        cout << "Masukkan nama server: ";
        getline(cin, name);

        int idx = -1;
        for (int i = 0; i < serverCount; ++i) {
            if (servers[i].nama == name) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            cout << "Server tidak ditemukan.\n";
            return;
        }

        if (servers[idx].pengguna_sekarang > 0) {
            cout << "Server masih memiliki pengguna aktif.\n";
            return;
        }

        for (int i = idx; i < serverCount - 1; ++i)
            servers[i] = servers[i + 1];
        serverCount--;
        cout << "Server berhasil dihapus.\n";
        saveData(); 
        cout << "" << endl;
        cout << "Apakah ingin hapus lagi? (y/n): ";
        cin >> loop;
        clearInputBuffer();
    } while (loop == "y");
    

}

void riwayat() {
    cout << "\n======================= RIWAYAT PENYEWAAN =======================\n";
    for (int i = 0; i < historyCount; ++i) {
        cout << histories[i].waktu << " - " << histories[i].aksi << endl;
    }
    cout << "" << endl;
    cout << "tekan enter untuk kembali";
    cin.get();
}

//===================================================user===================================
void requestAkun() {
    system("cls");
    if (currentUser.status == "active") {
        cout << "Anda sudah memiliki akun aktif.\n";
        cout << "" << endl;
        cout << "tekan enter untuk kembali";
        cin.get();
        return;
    }

    cout << "\n=== Request Server ===\n";
    cout << "Daftar Server:\n";
    for (int i = 0; i < serverCount; ++i)
        cout << servers[i].nama << " (" << servers[i].pengguna_sekarang << "/" << servers[i].kapasitas << ")\n";

    string server;
    cout << "Pilih server: ";
    getline(cin, server);

    int srvIdx = -1;
    for (int i = 0; i < serverCount; ++i) {
        if (servers[i].nama == server && servers[i].pengguna_sekarang < servers[i].kapasitas) {
            srvIdx = i;
            break;
        }
    }

    if (srvIdx == -1) {
        cout << "Server tidak tersedia atau penuh.\n";
        cin.get();
        return;
    }

    bool found = false;
    for (int i = 0; i < requestCount; ++i) {
        if (changeRequests[i].username == currentUser.username) {
            changeRequests[i].server = server;
            changeRequests[i].status = "pending";
            found = true;
            cout << "Permintaan berhasil diperbarui. Menunggu persetujuan admin.\n";
            break;
        }
    }

    if (!found) {
        changeRequests[requestCount++] = {
            currentUser.username,
            "",
            "",
            server,
            "pending"
        };
        cout << "Permintaan akun diajukan. Menunggu persetujuan admin.\n";
        cout << "" << endl;
        cout << "tekan enter untuk kembali";
        cin.get();
    }
    saveData();
}

void requestPerubahanAkun() {
    system("cls");
    ChangeRequest cr;
    cr.username = currentUser.username;

    cout << "Masukkan nama baru (kosongkan jika tidak ingin mengubah): ";
    getline(cin, cr.nama);
    cout << "Masukkan password baru (kosongkan jika tidak ingin mengubah): ";
    getline(cin, cr.password);

    cout << "\nDaftar Server Tersedia:\n";
    for (int i = 0; i < serverCount; ++i)
        cout << servers[i].nama << " (Kapasitas: " << servers[i].pengguna_sekarang << "/" << servers[i].kapasitas << ")\n";

    cout << "Masukkan server baru (kosongkan jika tidak ingin mengubah): ";
    getline(cin, cr.server);
    cr.status = "pending";

    changeRequests[requestCount++] = cr;
    cout << "Permintaan perubahan telah dikirim ke admin.\n";
    cout << "" << endl;
    cout << "tekan enter untuk kembali";
    cin.get();
    saveData();
}

void cekStatusAkun() {
    system("cls");
    cout << "\n=== STATUS AKUN ANDA ===\n";
    cout << "Username: " << currentUser.username << endl;
    cout << "Nama: " << currentUser.nama << endl;
    cout << "Status: " << currentUser.status << endl;
    cout << "Server: " << currentUser.server << endl;
    cout << "" << endl;
    cout << "tekan enter untuk kembali";
    cin.get();
}

void lihatServer() {
    system("cls");
    cout << "+----+--------------+------------+-----------------+" << endl;
    cout << "| ID | Nama Server  | Kapasitas  | Jumlah Pengguna |" << endl;
    cout << "+----+--------------+------------+-----------------+" << endl;
    for (int i = 0; i < serverCount; ++i)
    {
        cout << "| " << setw(2) << left << servers[i].id << " | ";
        cout << setw(12) << left << servers[i].nama.substr(0, 12) << " | ";
        cout << setw(10) << left << servers[i].kapasitas << " | ";
        cout << setw(15) << left << servers[i].pengguna_sekarang << " | \n";
        
    }
    cout << "+----+--------------+------------+-----------------+ \n";
    cout << "" << endl;
    cout << "tekan enter untuk kembali";
    cin.get();
}

int main() {
    loadData();

    while (true) {
        system("cls");
        cout << "\n=== SELAMAT DATANG DI SISTEM PENYEWAAN SERVER ===\n";
        cout << "1. Login\n";
        cout << "2. Registrasi\n";
        cout << "3. Keluar\n";
        cout << "===================================================" << endl;
        cout << "Pilihan: ";
        int pilih;
        cin >> pilih;
        clearInputBuffer();

        if (pilih == 1) {
            if (login()) {
                if (currentUser.username == "admin") {
                    // MENU ADMIN
                    while (true) {
                        system("cls");
                        cout << "\n=== MENU ADMIN ===\n";
                        cout << "1. Manajemen Akun User\n";
                        cout << "2. Manajemen Server\n";
                        cout << "3. Riwayat Penyewaan\n";
                        cout << "4. Kembali ke menu login\n";
                        cout << "5. Logout\n";
                        cout << "============================" << endl;
                        cout << "Pilihan: ";
                        int adminpilih;
                        cin >> adminpilih;
                        clearInputBuffer();

                        if (adminpilih == 1) {
                            while (true) {
                                system("cls");
                                cout << "\n=== MANAJEMEN PERMINTAAN USER ===\n";
                                cout << "1. Lihat Permintaan Pembuatan Akun Server\n";
                                cout << "2. Setujui Pembuatan Akun Server\n";
                                cout << "3. Lihat Permintaan Perubahan Akun\n";
                                cout << "4. Setujui Perubahan Akun\n";
                                cout << "5. Kembali\n";
                                cout << "=====================================" << endl;
                                cout << "Pilihan: ";
                                int sub;
                                cin >> sub;
                                clearInputBuffer();

                                if (sub == 1)
                                {
                                    lihatrequest();
                                }else if (sub == 2)
                                {
                                    setujuakun();
                                
                                }else if (sub == 3)
                                {
                                    lihatubah();
                                }else if (sub == 4)
                                {
                                    setujuubah();
                                }else if (sub ==5)
                                {
                                    break;
                                
                                }else{
                                    cout << "pilihan tidak valid" << endl;
                                }   
                            }
                        } else if (adminpilih == 2) {
                            while (true) {
                                system("cls");
                                cout << "\n=== MANAJEMEN SERVER ===\n";
                                cout << "1. Tambah Server\n";
                                cout << "2. Lihat Daftar Server\n";
                                cout << "3. Lihat Server Pengguna Terbanyak\n";
                                cout << "4. Ubah Server\n";
                                cout << "5. Hapus Server\n";
                                cout << "6. Kembali\n";
                                cout << "============================" << endl;
                                cout << "Pilihan: ";
                                int sub;
                                cin >> sub;
                                clearInputBuffer();

                                if (sub == 1)
                                {
                                    tambah();
                                }else if (sub == 2)
                                {
                                    lihat();
                                }else if (sub == 3)
                                {
                                    pengguna_terbanyak();
                                }else if (sub == 4)
                                {
                                    string name;
                                    
                                    editServer(name);
                                }else if (sub ==5)
                                {
                                    hapuserver();
                                }else if (sub == 6)
                                {
                                    
                                    break;
                                }else
                                {
                                    cout << "pilihan tidak valid" << endl;
                                }
                                
                            }
                        } else if (adminpilih == 3) {
                            riwayat();
                        } else if (adminpilih == 4) {
                            break;
                        } else if (adminpilih == 5) {
                            saveData();
                            cout << "terima kasih sudah berkunjung.\n";
                            return 0;
                        } else {
                            cout << "Pilihan tidak valid.\n";
                        }
                    }
                } else {
                    while (true) {
                        system("cls");
                        cout << "\n=== MENU USER ===\n";
                        cout << "1. Lihat Server\n";
                        cout << "2. Request Server\n";
                        cout << "3. Cek Status Akun\n";
                        cout << "4. Request Perubahan Akun\n";
                        cout << "5. Kembali ke menu login\n";
                        cout << "6. Logout\n";
                        cout << "============================" << endl;
                        cout << "Pilihan: ";
                        int userpilih;
                        cin >> userpilih;
                        clearInputBuffer();

                        if (userpilih == 1)
                        {
                            lihatServer();
                        }else if (userpilih == 2)
                        {
                            requestAkun();
                        }else if (userpilih == 3)
                        {
                            cekStatusAkun();
                        }else if (userpilih == 4)
                        {
                            requestPerubahanAkun();
                        }else if (userpilih == 5)
                        {
                            break;
                        }else if (userpilih == 6)
                        {
                            saveData();
                            cout << "terima kasih sudah berkunjung.\n";
                            return 0;
                        }else
                        {
                            cout << "pilihan tidak valid" << endl;
                        }
                    }
                }
            }
        } else if (pilih == 2) {
            registerUser();
        } else if (pilih == 3) {
            saveData();
            cout << "terima kasih sudah berkunjung.\n";
            return 0;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }

    return 0;
}