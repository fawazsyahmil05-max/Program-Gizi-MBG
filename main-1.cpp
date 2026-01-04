/*
================================================================================
   PROGRAM UTAMA : GIZI TRACKER ULTIMATE
   FILE          : main.cpp
   DESKRIPSI     : Multi-file implementation dengan KTP Lengkap & Save TXT
================================================================================
*/

#include <iostream>
#include <string>
#include "GiziLib.h"
#include "ktp.h"

using namespace std;

// PROTOTYPES
void initData();
void userDashboard();
void adminDashboard();
void menuRegister();
void menuLogin();

// GLOBAL OBJECTS
FoodAVL foodTree;            
AdminQueue deleteQueue;      
ActionStack historyStack;    
HuffmanAnalyst reporter;
CircularKTPList ktpDatabase; 

User users[50];              
MealHistory currentMealHist; 
int userCount = 0;
int loggedInIdx = -1;        

// =============================================================================
// MAIN FUNCTION
// =============================================================================
int main() {
    initData(); 
    int menu = -1;
    while(menu != 0) {
        clearScreen();
        cout << "=== GIZI TRACKER ULTIMATE ===\n";
        cout << "1. Login (NIK & Password)\n";
        cout << "2. Registrasi Data KTP (LENGKAP)\n";
        cout << "0. Keluar Aplikasi\n";
        cout << "Pilihan: ";
        
        menu = getIntInput();
        
        if(menu == 1) menuLogin();
        else if(menu == 2) menuRegister();
    }
    return 0;
}

// =============================================================================
// LOGIC IMPLEMENTATION
// =============================================================================

void initData() {
    // Load Dummy Data Makanan
    foodTree.addFood(1, "Nasi Putih", "Karbo", 130, 2.7, 28, 0.3);
    foodTree.addFood(2, "Dada Ayam", "Protein", 165, 31, 0, 3.6);
    foodTree.addFood(3, "Telur Rebus", "Protein", 78, 6, 0.6, 5);
    foodTree.addFood(4, "Tempe Goreng", "Protein", 35, 3, 2, 2.5);
    foodTree.addFood(5, "Tahu Goreng", "Protein", 35, 2.5, 1.5, 2.5);
    foodTree.addFood(6, "Apel Merah", "Buah", 52, 0.3, 14, 0.2);
    
    // Setup Admin (Hardcoded)
    users[0].id = 1; users[0].nama = "Admin Sistem"; users[0].nik = "admin";
    users[0].gender='L'; users[0].berat=70; users[0].tinggi=170; users[0].umur=25; 
    users[0].activityLevel=3; users[0].goal=2;
    users[0].hitungKebutuhan();
    userCount = 1; 
}

void menuLogin() {
    string nik, pass;
    cout << "\n=== LOGIN VIA KTP ===\n";
    cout << "NIK      : "; cin >> nik;
    cout << "Password : "; cin >> pass;
    
    // Cek Admin
    if(nik == "admin" && pass == "admin") {
        loggedInIdx = 0;
        adminDashboard();
        return;
    }

    // Cek di Database KTP (Circular LL)
    KTPData* ktpUser = ktpDatabase.loginSearch(nik, pass);
    
    if(ktpUser != NULL) {
        // Jika data KTP valid, cari data User Aplikasi-nya
        bool foundInApp = false;
        for(int i=0; i<userCount; i++) {
            if(users[i].nik == nik) {
                loggedInIdx = i;
                foundInApp = true;
                break;
            }
        }
        
        if(foundInApp) {
            cout << ">> Login Berhasil! Halo, " << ktpUser->nama << ".\n";
            pauseProgram();
            userDashboard();
        } else {
            cout << ">> Error: Data KTP ada tapi Data App korup/hilang.\n";
            pauseProgram();
        }
    } else {
        cout << ">> Login Gagal! NIK tidak ditemukan atau Password salah.\n";
        pauseProgram();
    }
}

void menuRegister() {
    if(userCount >= 50) {
        cout << "Database User Penuh (Max 50).\n";
        pauseProgram(); return;
    }

    KTPData newKtp;
    cout << "\n=== FORM REGISTRASI KTP LENGKAP ===\n";
    cout << "(Mohon isi data dengan benar untuk dicetak ke database_ktp.txt)\n\n";

    cout << "NIK (Nomor Induk) : "; cin >> newKtp.nik;
    if(ktpDatabase.isNikExist(newKtp.nik)) {
        cout << ">> ERROR: NIK sudah terdaftar!\n";
        pauseProgram(); return;
    }

    cout << "Nama Lengkap      : "; cin.ignore(); getline(cin, newKtp.nama);
    cout << "Tempat Lahir      : "; getline(cin, newKtp.tempatLahir);
    cout << "Tgl Lahir(DD-MM-YY): "; getline(cin, newKtp.tanggalLahir);
    cout << "Jenis Kelamin(L/P): "; getline(cin, newKtp.jenisKelamin);
    cout << "Golongan Darah    : "; getline(cin, newKtp.golDarah);
    cout << "Alamat (Jalan)    : "; getline(cin, newKtp.alamat);
    cout << "    RT/RW         : "; getline(cin, newKtp.rt_rw);
    cout << "    Kel/Desa      : "; getline(cin, newKtp.kel_desa);
    cout << "    Kecamatan     : "; getline(cin, newKtp.kecamatan);
    cout << "Agama             : "; getline(cin, newKtp.agama);
    cout << "Status Perkawinan : "; getline(cin, newKtp.statusPerkawinan);
    cout << "Pekerjaan         : "; getline(cin, newKtp.pekerjaan);
    cout << "Kewarganegaraan   : "; getline(cin, newKtp.kewarganegaraan);
    
    cout << "\n[SECURITY]\n";
    cout << "Buat Password App : "; cin >> newKtp.password;

    // SIMPAN KTP (Ke RAM & TXT)
    ktpDatabase.registerKTP(newKtp);

    // ISI DATA KESEHATAN (Untuk fitur Gizi Tracker)
    cout << "\n=== DATA KESEHATAN PENGGUNA ===\n";
    User u; 
    u.id = userCount + 1;
    u.nik = newKtp.nik;    
    u.nama = newKtp.nama;  
    
    cout << "Berat Badan (kg)  : "; cin >> u.berat;
    cout << "Tinggi Badan (cm) : "; cin >> u.tinggi;
    cout << "Umur (Tahun)      : "; cin >> u.umur;
    cout << "Gender (L/P)      : "; cin >> u.gender;
    cout << "Aktifitas (1-4)   : "; cin >> u.activityLevel;
    cout << "Goal (1-3)        : "; cin >> u.goal;
    
    u.hitungKebutuhan();
    users[userCount] = u;
    userCount++;
    
    cout << "\n>> REGISTRASI SUKSES!\n";
    cout << ">> Data KTP disimpan ke 'database_ktp.txt'.\n";
    cout << ">> Silakan Login menggunakan NIK & Password.\n";
    pauseProgram();
}

void userDashboard() {
    User* u = &users[loggedInIdx];
    int pil = -1;
    while(pil != 0) {
        historyStack.push("Dashboard User"); 
        clearScreen();
        cout << "=== DASHBOARD USER: " << u->nama << " ===\n";
        cout << "Target Kalori: " << u->targetKalori << " kkal/hari\n";
        cout << "Konsumsi Hari Ini: " << currentMealHist.getTotalKalori() << " kkal\n";
        cout << "----------------------------------\n";
        cout << "1. Catat Makan (Cari di AVL Tree)\n";
        cout << "2. Catat Minum Air\n";
        cout << "3. Laporan Mingguan & Grafik\n";
        cout << "4. Catat Tidur\n";
        cout << "5. Request Hapus Akun\n";
        cout << "6. Lihat History Navigasi\n";
        cout << "7. [EKSPOR] Simpan Laporan ke TXT\n";
        cout << "0. Logout\n";
        cout << "Pilihan: "; 
        
        pil = getIntInput();
        
        if(pil == 1) {
            clearScreen();
            foodTree.displayAll();
            cout << "ID Makanan: "; int fid = getIntInput();
            FoodItem* item = foodTree.findFood(fid);
            if(item) {
                currentMealHist.add(item->nama, item->kalori, "Makan");
                cout << ">> Berhasil dicatat.\n";
            } else cout << ">> ID Salah.\n";
            pauseProgram();
        }
        else if(pil == 2) {
            cout << "Tambah Air (Liter): "; double a; cin >> a; u->airKonsumsi += a;
            cout << ">> Total air: " << u->airKonsumsi << " L\n";
            pauseProgram();
        }
        else if(pil == 3) {
            cout << "=== WEEKLY LOG ===\n";
            for(int i=0; i<u->logCount; i++) 
                cout << "Hari " << u->weeklyLogs[i].hari << ": " << u->weeklyLogs[i].kaloriMasuk << " kkal\n";
            
            cout << "\n1. Tambah Data Harian? "; int x = getIntInput();
            if(x==1 && u->logCount < 7) {
                cout << "Hari (cth: Senin): "; cin >> u->weeklyLogs[u->logCount].hari;
                cout << "Berat (kg): "; cin >> u->weeklyLogs[u->logCount].berat;
                cout << "Kalori Masuk: "; cin >> u->weeklyLogs[u->logCount].kaloriMasuk;
                
                // Tentukan Status
                if(u->weeklyLogs[u->logCount].kaloriMasuk < u->targetKalori - 200) u->weeklyLogs[u->logCount].status = 1;
                else if(u->weeklyLogs[u->logCount].kaloriMasuk > u->targetKalori + 200) u->weeklyLogs[u->logCount].status = 3;
                else u->weeklyLogs[u->logCount].status = 2;
                
                u->logCount++;
                cout << ">> Data disimpan.\n";
            }
            pauseProgram();
        }
        else if(pil == 4) {
             cout << "Durasi Tidur (jam): "; cin >> u->jamTidur;
             pauseProgram();
        }
        else if(pil == 5) {
            deleteQueue.enqueue(u->nama, u->id);
            cout << ">> Request dikirim ke Admin.\n";
            pauseProgram();
        }
        else if(pil == 6) {
            historyStack.showHistory();
            pauseProgram();
        }
        else if(pil == 7) {
            // EKSPOR LAPORAN
            u->exportWeeklyReport();
            pauseProgram();
        }
    }
    currentMealHist.clear(); 
}

void adminDashboard() {
    int pil = -1;
    while(pil != 0) {
        historyStack.push("Dashboard Admin");
        clearScreen();
        cout << "=== ADMIN PANEL ===\n";
        cout << "1. Proses Antrian Hapus Akun\n";
        cout << "2. Laporan Efisiensi Huffman\n";
        cout << "3. Lihat Database Makanan\n";
        cout << "4. [EKSPOR] Database Makanan ke TXT\n";
        cout << "0. Logout\n";
        cout << "Pilihan: "; 
        
        pil = getIntInput();
        
        if(pil == 1) {
            if(!deleteQueue.isEmpty()) {
                string n; int i; deleteQueue.processRequest(n, i);
                cout << ">> Menghapus User: " << n << " (ID: " << i << ")...\n";
                cout << ">> User DIHAPUS.\n";
            } else cout << ">> Antrian Kosong.\n";
            pauseProgram();
        }
        else if(pil == 2) {
            string s = foodTree.getAllDataString();
            reporter.generateReport(s);
            pauseProgram();
        }
        else if(pil == 3) {
            foodTree.displayAll();
            pauseProgram();
        }
        else if(pil == 4) {
            foodTree.saveDatabaseToTxt();
            pauseProgram();
        }
    }
}
