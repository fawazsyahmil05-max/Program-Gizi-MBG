/*
================================================================================
   FILE   : GiziLib.h
   ISI    : FoodAVL, User Struct, Reporting Logic, Stack, Queue
================================================================================
*/

#ifndef GIZILIB_H
#define GIZILIB_H

#include <iostream>
#include <string>
#include <fstream> // Wajib untuk file .txt

using namespace std;

// ================= UTILITIES =================
void clearScreen() {
    for(int i = 0; i < 50; i++) cout << "\n";
}

void pauseProgram() {
    cout << "\n[TEKAN ENTER UNTUK LANJUT]";
    cin.ignore(1000, '\n');
    cin.get();
}

int getIntInput() {
    int input;
    while (!(cin >> input)) {
        cout << "Input salah! Masukkan angka: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return input;
}

void cetakSpasi(int totalLebar, int panjangKata) {
    int sisa = totalLebar - panjangKata;
    if (sisa < 1) sisa = 1;
    for(int i = 0; i < sisa; i++) cout << " ";
}

// ================= DATA MODELS =================

// 1. DATA MAKANAN
struct FoodItem {
    int id;
    string nama;
    string kategori;
    int kalori;
    double protein, karbo, lemak;
    FoodItem *left, *right;
    int height;
};

// 2. DATA USER (TERHUBUNG KE KTP VIA NIK)
struct User {
    int id;
    string nik;  // Foreign Key
    string nama; // Diambil dari KTP
    
    // Data Fisik untuk Hitung Gizi
    int umur;
    double tinggi, berat;
    char gender;
    int activityLevel, goal;
    double targetKalori, targetAir;
    double airKonsumsi, jamTidur;
    
    // Log Mingguan
    struct DailyLog {
        string hari;
        double berat;
        int kaloriMasuk;
        int status; 
    } weeklyLogs[7]; 
    int logCount;

    // Hitung Kebutuhan Kalori (Mifflin-St Jeor)
    void hitungKebutuhan() {
        double bmr;
        if (gender == 'L' || gender == 'l') 
            bmr = (10 * berat) + (6.25 * tinggi) - (5 * umur) + 5;
        else 
            bmr = (10 * berat) + (6.25 * tinggi) - (5 * umur) - 161;

        double multiplier = 1.2;
        if(activityLevel == 2) multiplier = 1.375;
        else if(activityLevel == 3) multiplier = 1.55;
        else if(activityLevel == 4) multiplier = 1.725;
        
        double tdee = bmr * multiplier;
        
        if (goal == 1) targetKalori = tdee - 500;
        else if (goal == 3) targetKalori = tdee + 500;
        else targetKalori = tdee;
        
        if (targetKalori < 1200) targetKalori = 1200;
        targetAir = (berat * 35) / 1000.0;
        airKonsumsi = 0; logCount = 0; jamTidur = 0;
    }

    // [FITUR] Export Laporan User ke TXT
    void exportWeeklyReport() {
        if(logCount == 0) {
            cout << ">> Data mingguan kosong. Tidak ada yang disimpan.\n";
            return;
        }
        string filename = "laporan_" + nama + ".txt";
        ofstream file(filename.c_str());

        if(file.is_open()) {
            file << "=== LAPORAN PROGRES MINGGUAN ===\n";
            file << "Nama User : " << nama << " (NIK: " << nik << ")\n";
            file << "Target    : " << targetKalori << " kkal/hari\n";
            file << "----------------------------------------------\n";
            file << "HARI      | BERAT  | ASUPAN KALORI | STATUS\n";
            file << "----------------------------------------------\n";
            for(int i=0; i<logCount; i++) {
                file << weeklyLogs[i].hari;
                for(int s=0; s<(10-weeklyLogs[i].hari.length()); s++) file << " ";
                file << "| " << weeklyLogs[i].berat << " kg | " << weeklyLogs[i].kaloriMasuk << " kkal    | ";
                if(weeklyLogs[i].status == 1) file << "Defisit";
                else if(weeklyLogs[i].status == 2) file << "Normal";
                else file << "Surplus";
                file << "\n";
            }
            file << "----------------------------------------------\n";
            file.close();
            cout << ">> Laporan berhasil disimpan ke file: " << filename << "\n";
        } else {
            cout << ">> Gagal membuat file.\n";
        }
    }
};

// 3. RIWAYAT MAKAN
struct MealNode {
    string nama; int kalori; string waktu;
    MealNode *next, *prev;
    MealNode(string n, int k, string w) { nama=n; kalori=k; waktu=w; next=NULL; prev=NULL; }
};

// ================= STRUKTUR DATA UTAMA =================

// [AVL TREE] Database Makanan
class FoodAVL {
private:
    FoodItem* root;
    int getHeight(FoodItem *N) { if (N == NULL) return 0; return N->height; }
    int getMaxVal(int a, int b) { return (a > b) ? a : b; }

    FoodItem* createNode(int id, string n, string cat, int k, double p, double c, double l) {
        FoodItem* node = new FoodItem();
        node->id = id; node->nama = n; node->kategori = cat;
        node->kalori = k; node->protein = p; node->karbo = c; node->lemak = l;
        node->left = NULL; node->right = NULL; node->height = 1;
        return node;
    }
    FoodItem* rightRotate(FoodItem* y) {
        FoodItem* x = y->left; FoodItem* T2 = x->right; x->right = y; y->left = T2;
        y->height = getMaxVal(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = getMaxVal(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }
    FoodItem* leftRotate(FoodItem* x) {
        FoodItem* y = x->right; FoodItem* T2 = y->left; y->left = x; x->right = T2;
        x->height = getMaxVal(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = getMaxVal(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }
    int getBalance(FoodItem* N) { if (N == NULL) return 0; return getHeight(N->left) - getHeight(N->right); }

    FoodItem* insertRec(FoodItem* node, int id, string n, string cat, int k, double p, double c, double l) {
        if (node == NULL) return(createNode(id, n, cat, k, p, c, l));
        if (id < node->id) node->left = insertRec(node->left, id, n, cat, k, p, c, l);
        else if (id > node->id) node->right = insertRec(node->right, id, n, cat, k, p, c, l);
        else return node;
        node->height = 1 + getMaxVal(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);
        if (balance > 1 && id < node->left->id) return rightRotate(node);
        if (balance < -1 && id > node->right->id) return leftRotate(node);
        if (balance > 1 && id > node->left->id) { node->left = leftRotate(node->left); return rightRotate(node); }
        if (balance < -1 && id < node->right->id) { node->right = rightRotate(node->right); return leftRotate(node); }
        return node;
    }
    void inOrderDisplay(FoodItem* root) {
        if(root != NULL) {
            inOrderDisplay(root->left);
            cout << "| " << root->id; if(root->id < 10) cout << "   | "; else cout << "  | ";
            cout << root->nama.substr(0, 19); cetakSpasi(20, root->nama.length()); cout << " | ";
            cout << root->kalori << " kkal |\n";
            inOrderDisplay(root->right);
        }
    }
    void traverseForString(FoodItem* node, string &fullText) {
        if(node != NULL) { traverseForString(node->left, fullText); fullText += node->nama; traverseForString(node->right, fullText); }
    }
    FoodItem* searchRec(FoodItem* root, int id) {
        if (root == NULL || root->id == id) return root;
        if (root->id < id) return searchRec(root->right, id);
        return searchRec(root->left, id);
    }
    // Helper untuk simpan ke TXT
    void saveRec(ofstream &file, FoodItem* node) {
        if(node != NULL) {
            saveRec(file, node->left);
            file << node->id << " | " << node->nama << " (" << node->kategori << ") | " 
                 << node->kalori << " kkal | P:" << node->protein << " C:" << node->karbo << " L:" << node->lemak << "\n";
            saveRec(file, node->right);
        }
    }

public:
    FoodAVL() { root = NULL; }
    void addFood(int id, string n, string cat, int k, double p, double c, double l) { root = insertRec(root, id, n, cat, k, p, c, l); }
    FoodItem* findFood(int id) { return searchRec(root, id); }
    void displayAll() {
        cout << "-------------------------------------\n"; cout << "| ID  | NAMA MAKANAN         | KALORI |\n"; cout << "-------------------------------------\n";
        inOrderDisplay(root); cout << "-------------------------------------\n";
    }
    string getAllDataString() { string s = ""; traverseForString(root, s); return s; }
    
    // [FITUR] Simpan Database Makanan ke TXT
    void saveDatabaseToTxt() {
        ofstream file("laporan_makanan.txt");
        if(file.is_open()) {
            file << "=== DATABASE MAKANAN GIZI TRACKER ===\n";
            file << "ID | NAMA | KALORI | MAKRO\n";
            file << "-------------------------------------\n";
            saveRec(file, root);
            file << "-------------------------------------\n";
            file.close();
            cout << ">> Database berhasil diekspor ke 'laporan_makanan.txt'\n";
        } else {
            cout << ">> Gagal membuka file.\n";
        }
    }
};

// [QUEUE] Admin Approval
struct QueueNode { string username; int userId; QueueNode* next; };
class AdminQueue {
private: QueueNode *front, *rear;
public:
    AdminQueue() { front = rear = NULL; }
    void enqueue(string name, int id) {
        QueueNode* temp = new QueueNode(); temp->username = name; temp->userId = id; temp->next = NULL;
        if (rear == NULL) { front = rear = temp; return; }
        rear->next = temp; rear = temp;
    }
    bool processRequest(string &outName, int &outId) {
        if (front == NULL) return false;
        QueueNode* temp = front; outName = front->username; outId = front->userId;
        front = front->next; if (front == NULL) rear = NULL; delete temp; return true;
    }
    bool isEmpty() { return front == NULL; }
};

// [STACK] History Navigasi
struct StackNode { string activity; StackNode* next; };
class ActionStack {
private: StackNode* top;
public:
    ActionStack() { top = NULL; }
    void push(string act) { StackNode* n = new StackNode(); n->activity = act; n->next = top; top = n; }
    void showHistory() {
        cout << "\n[STACK] Riwayat Aktivitas (LIFO):\n"; StackNode* t = top; int i=1;
        if(t==NULL) cout << "(Kosong)\n"; while(t) { cout << i++ << ". " << t->activity << "\n"; t = t->next; }
    }
};

// [HUFFMAN] Reporting Simulation
class HuffmanAnalyst {
public:
    void generateReport(string text) {
        if(text.length() == 0) { cout << "Data kosong.\n"; return; }
        int freq[256] = {0}; for (size_t i=0; i<text.length(); i++) freq[(int)text[i]]++;
        long originalBits = text.length() * 8; long compressedBits = 0;
        for(int i=0; i<256; i++) { if(freq[i] > 0) { int bitLen = (freq[i] > (int)text.length()/10) ? 3 : 8; compressedBits += (freq[i] * bitLen); } }
        double saving = ((double)(originalBits - compressedBits) / originalBits) * 100.0;
        cout << "\n=== LAPORAN HUFFMAN ===\n"; cout << "Total Karakter  : " << text.length() << " chars\n";
        cout << "Ukuran Asli     : " << originalBits << " bits\n"; cout << "Ukuran Kompresi : ~" << compressedBits << " bits\n";
        cout << "Penghematan     : " << (int)saving << " %\n"; cout << "=======================\n";
    }
};

// [LINKED LIST] Riwayat Makan Sesi Ini
class MealHistory {
private: MealNode *head, *tail;
public:
    MealHistory() { head = tail = NULL; }
    void add(string n, int k, string w) { MealNode* node = new MealNode(n, k, w); if(!head) { head = tail = node; } else { tail->next = node; node->prev = tail; tail = node; } }
    int getTotalKalori() { int t = 0; MealNode* temp = head; while(temp) { t += temp->kalori; temp = temp->next; } return t; }
    void clear() { while(head) { MealNode* temp = head; head = head->next; delete temp; } tail = NULL; }
};

#endif
