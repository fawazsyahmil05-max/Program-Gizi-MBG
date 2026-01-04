/*
================================================================================
   FILE   : ktp.h
   ISI    : Struct KTP Lengkap & Circular Linked List & Save to TXT
================================================================================
*/

#ifndef KTP_H
#define KTP_H

#include <iostream>
#include <string>
#include <fstream> // Wajib untuk file .txt

using namespace std;

// 1. DATA KTP (LENGKAP SESUAI FISIK KTP INDONESIA)
struct KTPData {
    string nik;
    string nama;
    string tempatLahir;
    string tanggalLahir; // Format DD-MM-YYYY
    string jenisKelamin;
    string golDarah;
    string alamat;
    string rt_rw;
    string kel_desa;
    string kecamatan;
    string agama;
    string statusPerkawinan;
    string pekerjaan;
    string kewarganegaraan;
    string password; // Tambahan untuk Login App
};

// 2. NODE CIRCULAR LINKED LIST
struct KTPNode {
    KTPData data;
    KTPNode* next;
};

// 3. CLASS CIRCULAR LINKED LIST
class CircularKTPList {
private:
    KTPNode* head;

public:
    CircularKTPList() { head = NULL; }

    // Fungsi Registrasi: Simpan ke Linked List & File TXT
    void registerKTP(KTPData newData) {
        // --- A. SIMPAN KE MEMORI (CIRCULAR LINKED LIST) ---
        KTPNode* newNode = new KTPNode();
        newNode->data = newData;
        newNode->next = NULL;

        if (head == NULL) {
            head = newNode;
            newNode->next = head; // Point ke diri sendiri (Circular)
        } else {
            KTPNode* temp = head;
            while (temp->next != head) { // Traverse ke node terakhir
                temp = temp->next;
            }
            temp->next = newNode;
            newNode->next = head; // Node terakhir kembali ke head
        }

        // --- B. SIMPAN KE FILE TXT (DATABASE PERMANEN) ---
        ofstream file("database_ktp.txt", ios::app); // Mode Append
        if (file.is_open()) {
            file << "==========================================\n";
            file << "NIK          : " << newData.nik << "\n";
            file << "PASSWORD     : " << newData.password << "\n"; // Disimpan agar admin bisa cek
            file << "Nama         : " << newData.nama << "\n";
            file << "TTL          : " << newData.tempatLahir << ", " << newData.tanggalLahir << "\n";
            file << "Jenis Kelamin: " << newData.jenisKelamin << "\n";
            file << "Gol. Darah   : " << newData.golDarah << "\n";
            file << "Alamat       : " << newData.alamat << "\n";
            file << "    RT/RW    : " << newData.rt_rw << "\n";
            file << "    Kel/Desa : " << newData.kel_desa << "\n";
            file << "    Kecamatan: " << newData.kecamatan << "\n";
            file << "Agama        : " << newData.agama << "\n";
            file << "Status       : " << newData.statusPerkawinan << "\n";
            file << "Pekerjaan    : " << newData.pekerjaan << "\n";
            file << "Kewarganegaraan: " << newData.kewarganegaraan << "\n";
            file << "==========================================\n";
            file.close();
            cout << ">> [SYSTEM] Data KTP berhasil disimpan ke 'database_ktp.txt'\n";
        } else {
            cout << ">> [ERROR] Gagal menulis ke file txt.\n";
        }
    }

    // Fungsi Login (Mencari di Circular List)
    KTPData* loginSearch(string nikInput, string passwordInput) {
        if (head == NULL) return NULL;

        KTPNode* temp = head;
        do {
            if (temp->data.nik == nikInput) {
                if(temp->data.password == passwordInput) {
                    return &(temp->data); // Ketemu & Password Benar
                } else {
                    return NULL; // Password Salah
                }
            }
            temp = temp->next;
        } while (temp != head); // Berhenti jika kembali ke head

        return NULL; // Tidak ketemu
    }

    // Cek Duplikasi NIK
    bool isNikExist(string nikInput) {
        if (head == NULL) return false;
        KTPNode* temp = head;
        do {
            if (temp->data.nik == nikInput) return true;
            temp = temp->next;
        } while (temp != head);
        return false;
    }
};

#endif
