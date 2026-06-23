#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  BASE CLASS: Encryption
//  Defines the interface for all cipher types.
//  This enables POLYMORPHISM — you can swap
//  XORCipher for CaesarCipher without changing
//  the rest of the code.
// ─────────────────────────────────────────────
class Encryption {
public:
    virtual string encrypt(const string& text) const = 0;  // pure virtual
    virtual string decrypt(const string& text) const = 0;  // pure virtual
    virtual ~Encryption() {}
};


// ─────────────────────────────────────────────
//  DERIVED CLASS: XORCipher
//  XOR flips bits using a key character.
//  XOR is self-reversing: encrypt(encrypt(x)) == x
//  So the same function works for both encrypt & decrypt!
// ─────────────────────────────────────────────
class XORCipher : public Encryption {
private:
    char key;  // the secret key used for XOR

public:
    XORCipher(char k = '#');  // default key is '#'
    string encrypt(const string& text) const override;
    string decrypt(const string& text) const override;
};


// ─────────────────────────────────────────────
//  DERIVED CLASS: CaesarCipher
//  Shifts each character by a fixed number (shift).
//  Classic substitution cipher.
// ─────────────────────────────────────────────
class CaesarCipher : public Encryption {
private:
    int shift;

public:
    CaesarCipher(int s = 3);  // default shift of 3
    string encrypt(const string& text) const override;
    string decrypt(const string& text) const override;
};

#endif