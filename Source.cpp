#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <thread>

using namespace std;

// modul de stocare a datelor in sql
enum Sql_TYPE
{
    Sql_TEXT = 0, Sql_INT = 1, Sql_FLOAT = 2
};

// pentru a stoca valoarea
class Valoare
{
    Sql_TYPE tip = Sql_TEXT;
    int dim = 0;
    int adim = 0;
    char* text = NULL;

    void copiere(Sql_TYPE tip, int dim, char* text)
    {
        if ((dim <= 0) || (text == NULL))
        {
            Sql_TYPE tip = Sql_TEXT; // tip data
            this->dim = 0; // dimensiune maxima
            this->adim = 0; // dimensiune actuala
            this->text = NULL; // data
        }
        else
        {
            if (dim > 255)
            {
                throw new runtime_error("Valoarea are dimensiune mai mare de 255");
            }
            this->dim = dim;
            this->adim = strlen(text) + 1;
            if (adim - 1 > dim)
            {
                throw new runtime_error("Valoarea asteptata are o dimensiune mai mare decat cea maxima");
            }
            this->text = new char[adim];
            if (tip == 0)
            {
                // text
                for (int i = 0; i < this->adim; i++)
                {
                    this->text[i] = text[i];
                }
            }
            if (tip == 1)
            {
                // int
                for (int i = 0; i < this->adim; i++)
                {
                    this->text[i] = text[i];
                    if (!(text[i] >= '0' && text[i] <= '9') && i != this->adim - 1)
                    {
                        throw new runtime_error("Se asteapta o valoare de tip int dar s-a primit altceva");
                    }
                }
            }
            if (tip == 2)
            {
                // float
                for (int i = 0; i < this->adim; i++)
                {
                    this->text[i] = text[i];
                    if (!((text[i] >= '0' && text[i] <= '9') || text[i] == '.') && i != this->adim - 1)
                    {
                        throw new runtime_error("Se asteapta o valoare de tip float dar s-a primit altceva");
                    }
                }
                if (text[this->adim - 2] == '.')
                {
                    throw new runtime_error("Valoarea float nu are cum sa aiba virgula dinaintea partii fractionare ca ultim caracter");
                }
            }
        }
    }

    void stergere()
    {
        delete[] this->text;
    }

public:
    Valoare()
    {
        // preinitializat
    }

    Valoare(Sql_TYPE tip, int dim, char* text)
    {
        copiere(tip, dim, text);
    }

    Valoare(Sql_TYPE tip, int dim, string text)
    {
        char* chText = new char[text.length() + 1];
        strcpy(chText, text.data());
        copiere(tip, dim, chText);
        delete[] chText;
    }

    Valoare(const Valoare& w)
    {
        copiere(w.tip, w.dim, w.text);
    }

    Valoare& operator=(const Valoare& w)
    {
        if (this != &w)
        {
            stergere();
            copiere(w.tip, w.dim, w.text);
        }
        return *this;
    }

    operator int()
    {
        if (tip != Sql_INT)
        {
            throw new runtime_error("Conversie de la Valoare non-int la int!");
        }
        return *((int*)text);
    }

    operator string()
    {
        if (tip != Sql_TEXT)
        {
            throw new runtime_error("Conversie de la Valoare non-text la text!");
        }
        return string(text);
    }

    friend ostream& operator<<(ostream& out, const Valoare& w)
    {
        switch (w.tip)
        {
        case Sql_TEXT:
            for (int i = 0; i < strlen(w.text) + 1; i++)
            {
                out << w.text[i];
            }
            break;
        case Sql_INT:
            out << *((int*)w.text);
            break;
        case Sql_FLOAT:
            out << *((float*)w.text);
            break;
        }
        return out;
    }

    ~Valoare()
    {
        stergere();
    }

    int getAdim()
    {
        return this->adim;
    }

    char* getText()
    {
        return this->text;
    }
};

// pentru a stoca coloanele
class Coloana
{
    string denumire;
    Sql_TYPE tip;
    int      dim;
    Valoare  defval;

    void copiere(string denumire, Sql_TYPE tip, int dim, Valoare defval)
    {
        this->denumire = denumire;
        this->tip = tip;
        this->dim = dim;
        this->defval = defval;
    }
public:
    Coloana()
    {
        // preinitializat
    }
    Coloana(string denumire)
    {
        this->denumire = denumire;
    }

    Coloana(string denumire, Sql_TYPE tip, int dim, Valoare defval)
    {
        copiere(denumire, tip, dim, defval);
    }

    Coloana(const Coloana& c)
    {
        copiere(c.denumire, c.tip, c.dim, c.defval);
    }

    Coloana& operator=(const Coloana& c)
    {
        copiere(c.denumire, c.tip, c.dim, c.defval);
        return *this;
    }

    string getDenumire()
    {
        return this->denumire;
    }

    int getTip()
    {
        return (int)this->tip;
    }

    int getDim()
    {
        return this->dim;
    }

    Valoare getVal()
    {
        return this->defval;
    }
    friend ostream& operator<<(ostream& out, const Coloana& c)
    {
        // afiseaza date
        out << "\n  Denumire: " << c.denumire;
        out << "\n  Tip: ";
        if (c.tip == 0)
        {
            cout << "text";
        }
        else if (c.tip == 1)
        {
            cout << "int";
        }
        else
        {
            cout << "float";
        }
        cout << "\n  Dimensiune: " << c.dim;
        cout << "\n  Valoare predefinita: " << c.defval;
        cout << endl;
        return out;
    }

    friend ofstream& operator<<(ofstream& out, const Coloana& c)
    {
        // afiseaza date fisier text
        out << "\n  Denumire: " << c.denumire;
        out << "\n  Tip: ";
        if (c.tip == 0)
        {
            cout << "text";
        }
        else if (c.tip == 1)
        {
            cout << "int";
        }
        else
        {
            cout << "float";
        }
        cout << "\n  Dimensiune: " << c.dim;
        cout << "\n  Valoare predefinita: " << c.defval;
        cout << endl;
        return out;
    }
};

// pentru a stoca tabelele
class Tabela
{
    string numeTabela;
    int nrColoane = 0;
    Coloana** listaColoane = NULL;

    void copiere(string numeTabela, int nrColoane, Coloana** listaColoane)
    {
        this->numeTabela = numeTabela;
        if ((nrColoane <= 0) || (listaColoane == NULL))
        {
            this->nrColoane = 0;
            this->listaColoane = NULL;
        }
        else
        {
            this->nrColoane = nrColoane;
            this->listaColoane = new Coloana * [this->nrColoane];
            for (int i = 0; i < this->nrColoane; i++)
            {
                this->listaColoane[i] = new Coloana(*listaColoane[i]);
            }
        }
    }

    void stergere()
    {
        for (int i = 0; i < this->nrColoane; i++)
        {
            delete this->listaColoane[i];
        }
        delete[] this->listaColoane;
    }
public:
    Tabela()
    {
        // preinitializat
    }

    Tabela(string numeTabela, int nrColoane, Coloana** listaColoane)
    {
        copiere(numeTabela, nrColoane, listaColoane);
    }

    Tabela(const Tabela& w)
    {
        copiere(w.numeTabela, w.nrColoane, w.listaColoane);
    }

    Tabela& operator=(const Tabela& w)
    {
        if (this != &w)
        {
            stergere();
            copiere(w.numeTabela, w.nrColoane, w.listaColoane);
        }
        return *this;
    }

    friend ostream& operator<<(ostream& out, const Tabela& w) {
        out << "\nNr Coloane: " << w.nrColoane;
        for (int i = 0; i < w.nrColoane; i++)
        {
            out << *w.listaColoane[i];
        }
        return out;
    }

    bool operator!()
    {
        return this->listaColoane == NULL;
    }

    // tabela + coloana -> adauga coloana in tabel
    Tabela& operator+(const Coloana& c)
    {
        Tabela copie(*this);
        stergere();
        this->nrColoane++;
        this->listaColoane = new Coloana * [this->nrColoane];
        for (int i = 0; i < this->nrColoane - 1; i++)
        {
            this->listaColoane[i] = new Coloana(*copie.listaColoane[i]);
        }
        this->listaColoane[this->nrColoane - 1] = new Coloana(c);
        return *this;
    }

    string getNumeTabela()
    {
        return this->numeTabela;
    }

    int getNrColoane()
    {
        return this->nrColoane;
    }

    Coloana* getColoana(int index)
    {
        return this->listaColoane[index];
    }

    void setNumeTabela(string numeTabela)
    {
        this->numeTabela = numeTabela;
    }

    ~Tabela()
    {
        stergere();
    }
};

class ListaCuvinte
{
public:
    string cuvant;
    ListaCuvinte* next = NULL;
    ListaCuvinte(string s)
    {
        // gata pentru implementarea recursiva
        cuvant = s;
        next = NULL;
    }

    friend ostream& operator<<(ostream& out, const ListaCuvinte& w)
    {
        const ListaCuvinte* r = &w;
        if (r != NULL)
        {
            out << r->cuvant;
            r = r->next;
        }

        // se parcurge fiecare cuvant
        while (r != NULL)
        {
            out << ", " << r->cuvant;
            r = r->next;
        }
        return out;
    }
};

// dintr-o linie se face o lista de cuvinte
// implementat recursiv, pana se ajunge la final
// considera parantezele, virgula si egalul ca fiind cuvinte (pentru validare functii in clase)
ListaCuvinte* mklista(string s)
{
    ListaCuvinte* r = NULL;
    // se cauta primul despartitor
    int k = s.find_first_of(" (),=");

    // daca se gaseste un singur cuvant
    if (k == string::npos)
    {
        if (s.size() > 0)
        {
            r = new ListaCuvinte(s);
        }
        return r;
    }

    // daca se gaseste spatiu
    if (s[k] == ' ')
    {
        if (k == 0)
        {
            // cazul " CREATE"
            return mklista(s.substr(1));
        }
        // cazul "CREATE TABLE"
        r = new ListaCuvinte(s.substr(0, k));
        r->next = mklista(s.substr(k + 1));
        return r;
    }

    if (k == 0)
    {
        // cazul ",TABLE"
        r = new ListaCuvinte(string(1, s[k]));
        r->next = mklista(s.substr(1));
        return r;
    }

    // cazul "CREATE,TABLE"
    r = new ListaCuvinte(s.substr(0, k));
    r->next = new ListaCuvinte(string(1, s[k]));
    r->next->next = mklista(s.substr(k + 1));
    return r;
}

// clasa care gestioneaza fisierele
class gestiuneFisiere
{
    // dpc = display table counter, sc = select counter (pt numerotare fisiere)
    static int dpc;
    static int sc;
public:
    // returneaza poz de la care incep datele efective in fisier
    static int getStartOffset(string numeFisier)
    {
        fstream f(numeFisier);
        char test;
        while (!f.eof())
        {
            f.read((char*)&test, sizeof(char));
            if (test == '~')
            {
                int startOffset = f.tellp();
                f.close();
                return startOffset;
            }
        }
        throw new runtime_error("Fisier tabela corupt!");
    }

    // returneaza poz ultimului octet
    static int getFinal(string numeFisier)
    {
        fstream f(numeFisier);
        f.seekp(0, ios::end);
        int final = f.tellp();
        f.close();
        return final;
    }

    // verifica daca exista tabela
    static bool existaFisier(string numeFisier)
    {
        fstream file(numeFisier);
        if (file.is_open())
        {
            file.close();
            return true;
        }
        return false;
    }

    // recupereaza tabela dintr-un fisier text
    static Tabela retrieveTable(string numeFisier)
    {
        // daca exista acel fisier
        if (gestiuneFisiere::existaFisier(numeFisier))
        {
            ifstream f(numeFisier);
            int length;
            f.read((char*)&length, sizeof(int)); // dimensiune numeTabela
            char* buffer;
            buffer = new char[length];
            f.read(buffer, length * sizeof(char)); // numeTabela
            string numeTabela = buffer; // 100% ok pana aici (verificat)
            delete[] buffer;
            int nrColoane;
            f.read((char*)&nrColoane, sizeof(int)); // nrColoane
            string* denCol = new string[nrColoane]; // denumiri Coloane
            int* dataType = new int[nrColoane]; // datatype Coloana
            int* dimVal = new int[nrColoane]; // dim max Valoare
            int* adim = new int[nrColoane]; // dim actuala Valoare
            string* textValue = new string[nrColoane]; // Valoare (stocata in text)
            Valoare** valori = new Valoare * [nrColoane]; // Valori (clasa efectiva)
            Coloana** coloane = new Coloana * [nrColoane]; // Coloane (clasa efectiva)
            for (int i = 0; i < nrColoane; i++)
            {
                f.read((char*)&length, sizeof(int)); // dimensiune denCol
                buffer = new char[length];
                f.read(buffer, length * sizeof(char)); // dencol
                denCol[i] = buffer;
                delete[] buffer;
                f.read((char*)&dataType[i], sizeof(int)); // dataType
                f.read((char*)&dimVal[i], sizeof(int)); // dim max valoare
                f.read((char*)&adim[i], sizeof(int)); // adim
                buffer = new char[adim[i]];
                f.read(buffer, adim[i] * sizeof(char)); // valoare (stocata in text)
                textValue[i] = buffer;
                delete[] buffer;
                valori[i] = new Valoare((Sql_TYPE)dataType[i], dimVal[i], textValue[i]);
                coloane[i] = new Coloana(denCol[i], (Sql_TYPE)dataType[i], dimVal[i], *valori[i]);
            }
            delete[] denCol;
            delete[] dataType;
            delete[] dimVal;
            delete[] adim;
            delete[] textValue;
            Tabela t(numeTabela, nrColoane, coloane);
            for (int i = 0; i < nrColoane; i++)
            {
                delete valori[i];
                delete coloane[i];
            }
            delete[] valori;
            delete[] coloane;
            char test;
            f.read((char*)&test, sizeof(char));
            f.close();
            // test intern (foarte folositor pt debug, mereu dupa ~ incep valorile efective)
            if (test == '~')
            {
                return t;
            }
            else
            {
                throw new runtime_error("Fisier tabela corupt!");
            }
        }
        else
        {
            throw new runtime_error("Nu exista fisierul din care se incearca recuperarea structurii tabelului cerut!");
        }
    }

    // creeaza fisierul pentru tabela
    static void createTable(Tabela& t)
    {
        ofstream f(t.getNumeTabela() + ".bin"); // doar pentru stocarea datelor
        int length = t.getNumeTabela().length() + 1; // dimensiune string
        f.write((char*)&length, sizeof(int)); // scriere dimensiune string
        f.write((char*)t.getNumeTabela().data(), length * sizeof(char)); // scriere string propriu zisa
        int nrColoane = t.getNrColoane(); // recupereaza numarul de coloane
        f.write((char*)&nrColoane, sizeof(int)); // scriere numar coloane
        for (int i = 0; i < nrColoane; i++) // loop scriere coloane
        {
            string denCol = t.getColoana(i)->getDenumire(); // recupereaza denumirea coloanei
            length = denCol.length() + 1; // dimensiune nume
            f.write((char*)&length, sizeof(int)); // scriere dimensiune string
            f.write((char*)denCol.data(), length * sizeof(char)); // scriere string propriu zisa
            int dataType = t.getColoana(i)->getTip(); // recupereaza tipul valorii
            f.write((char*)&dataType, sizeof(int)); // scriere tip valoare
            int dimVal = t.getColoana(i)->getDim(); // recuperare dimensiune maxima valoare
            f.write((char*)&dimVal, sizeof(int)); // scriere dimensiune maxima valoare
            Valoare defValue = t.getColoana(i)->getVal(); // recuperare valoare
            length = defValue.getAdim(); // dimensiune valoare reala
            f.write((char*)&length, sizeof(int)); // scriere dimensiune valoare
            string text = defValue.getText(); // conversie la string pentru scriere mai usoara
            f.write((char*)text.data(), length * sizeof(char)); // scriere valoare
        }
        char final = '~'; // marcator final de structura tabela
        f.write((char*)&final, sizeof(char)); // scriere marcator
        f.close();
    }

    // adauga valori in fisierul binar
    static void addValues(string numeFisier, ListaCuvinte* values)
    {
        fstream f(numeFisier);
        // se duce la final de fisier
        f.seekp(0, ios::end);
        // incepe sa scrie
        while (values != NULL)
        {
            string value = values->cuvant;
            int length = value.length() + 1;
            f.write((char*)&length, sizeof(int));
            f.write((char*)value.data(), length * sizeof(char));
            values = values->next;
        }
        f.close();
    }

    static void displayTableConsola(string numeFisier)
    {
        // se recupereaza structura tabelei din fisierul binar si scrierea structurii ei
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        cout << "TABELA: " << t.getNumeTabela() << endl;
        for (int i = 0; i < t.getNrColoane(); i++)
        {
            cout << t.getColoana(i)->getDenumire() << " | ";
        }
        cout << endl;
        for (int i = 0; i < t.getNrColoane(); i++)
        {
            for (int j = 0; j < t.getColoana(i)->getDenumire().length() + 3; j++)
            {
                cout << "-";
            }
        }
        cout << endl;
        int startOffset = gestiuneFisiere::getStartOffset(numeFisier);
        int final = gestiuneFisiere::getFinal(numeFisier);
        fstream f(numeFisier);
        f.seekp(startOffset);
        // se parcurg toate valorile si se afiseaza pe ecran
        while (f.tellp() != final)
        {
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                string value;
                char* buffer;
                int length;
                f.read((char*)&length, sizeof(int));
                buffer = new char[length];
                f.read(buffer, length * sizeof(char));
                value = buffer;
                delete[] buffer;
                cout << value << " | ";
            }
            cout << "\n";
        }
        f.close();
    }

    // copy paste displayTable doar ca scrie in fisier in loc de consola
    static void displayTableFisier(string numeFisier)
    {
        string numeDP = "DISPLAY TABLE_" + to_string(dpc) + ".txt";
        ofstream dp(numeDP);
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        if (dp.is_open())
        {
            dp << "TABELA: " << t.getNumeTabela() << endl;
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                dp << t.getColoana(i)->getDenumire() << " | ";
            }
            dp << endl;
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                for (int j = 0; j < t.getColoana(i)->getDenumire().length() + 3; j++)
                {
                    dp << "-";
                }
            }
            dp << endl;
            int startOffset = gestiuneFisiere::getStartOffset(numeFisier);
            int final = gestiuneFisiere::getFinal(numeFisier);
            fstream f(numeFisier);
            f.seekp(startOffset);
            while (f.tellp() != final)
            {
                for (int i = 0; i < t.getNrColoane(); i++)
                {
                    string value;
                    char* buffer;
                    int length;
                    f.read((char*)&length, sizeof(int));
                    buffer = new char[length];
                    f.read(buffer, length * sizeof(char));
                    value = buffer;
                    delete[] buffer;
                    dp << value << " | ";
                }
                dp << "\n";
            }
            f.close();
            dp.close();
            dpc++;
            cout << "RAPORTUL PENTRU DISPLAY TABLE SE POATE REGASI IN: " << numeDP << endl;
        }
        else
        {
            throw new runtime_error("Fisierul text pentru Display Table nu a putut fi creat!");
        }
    }

    // comanda select (afisare ecran)
    static void select(string numeFisier, int nrColoane, int* ordineColoane, bool where, int nrColoanaConditieWhere, string valoareColoanaConditieWhere)
    {
        // recuperare tabela din fisierul binar si scrierea structurii ei
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        cout << "TABELA: " << t.getNumeTabela() << endl;
        for (int i = 0; i < nrColoane; i++)
        {
            cout << t.getColoana(ordineColoane[i])->getDenumire() << " | ";
        }
        cout << endl;
        for (int i = 0; i < nrColoane; i++)
        {
            for (int j = 0; j < t.getColoana(ordineColoane[i])->getDenumire().length() + 3; j++)
            {
                cout << "-";
            }
        }
        cout << endl;
        int startOffset = gestiuneFisiere::getStartOffset(numeFisier);
        int final = gestiuneFisiere::getFinal(numeFisier);
        string* values;
        fstream f(numeFisier);
        f.seekp(startOffset);
        // se parcurge fisierul
        while (f.tellp() != final)
        {
            // string alocat dinamic pt a extrage valorile din fisier (se extrage linie cu linie daca e privit ca o tabela)
            values = new string[t.getNrColoane()];
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                char* buffer;
                int length;
                f.read((char*)&length, sizeof(int));
                buffer = new char[length];
                f.read(buffer, length * sizeof(char));
                values[i] = buffer;
                delete[] buffer;
            }
            // daca nu exista conditie de where in select se afiseaza pur si simplu
            if (!where)
            {
                for (int i = 0; i < nrColoane; i++)
                {
                    cout << values[ordineColoane[i]] << " | ";
                }
                cout << endl;
            }
            else
            {
                // altfel, se verifica daca conditia este indeplinita
                if (values[nrColoanaConditieWhere] == valoareColoanaConditieWhere)
                {
                    // si daca este indeplinita, atunci se afiseaza
                    for (int i = 0; i < nrColoane; i++)
                    {
                        cout << values[ordineColoane[i]] << " | ";
                    }
                    cout << endl;
                }
            }
            // stergere valori pentru a putea citi un nou rand de valori
            delete[] values;
        }
    }

    // comanda select (afisare fisier) 
    static void selectFisier(string numeFisier, int nrColoane, int* ordineColoane, bool where, int nrColoanaConditieWhere, string valoareColoanaConditieWhere)
    {
        // aceleasi principii ca la selectul pt consola
        string numeS = "SELECT_" + to_string(sc) + ".txt";
        ofstream s(numeS);
        fstream f(numeFisier);
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        if (s.is_open())
        {
            // antet
            s << "TABELA: " << t.getNumeTabela() << endl;
            for (int i = 0; i < nrColoane; i++)
            {
                s << t.getColoana(ordineColoane[i])->getDenumire() << " | ";
            }
            s << endl;
            for (int i = 0; i < nrColoane; i++)
            {
                for (int j = 0; j < t.getColoana(ordineColoane[i])->getDenumire().length() + 3; j++)
                {
                    s << "-";
                }
            }
            s << endl;
            int startOffset = gestiuneFisiere::getStartOffset(numeFisier);
            int final = gestiuneFisiere::getFinal(numeFisier);
            string* values;
            f.seekp(startOffset);
            while (f.tellp() != final)
            {
                values = new string[t.getNrColoane()];
                for (int i = 0; i < t.getNrColoane(); i++)
                {
                    char* buffer;
                    int length;
                    f.read((char*)&length, sizeof(int));
                    buffer = new char[length];
                    f.read(buffer, length * sizeof(char));
                    values[i] = buffer;
                    delete[] buffer;
                }
                if (!where)
                {
                    for (int i = 0; i < nrColoane; i++)
                    {
                        s << values[ordineColoane[i]] << " | ";
                    }
                    s << endl;
                }
                else
                {
                    if (values[nrColoanaConditieWhere] == valoareColoanaConditieWhere)
                    {
                        for (int i = 0; i < nrColoane; i++)
                        {
                            s << values[ordineColoane[i]] << " | ";
                        }
                        s << endl;
                    }
                }
                delete[] values;
            }
        }
        else
        {
            throw new runtime_error("Fisierul text pentru SELECT nu a putut fi creat!");
        }
        f.close();
        s.close();
        sc++;
        cout << "RAPORTUL PENTRU DISPLAY TABLE SE POATE REGASI IN: " << numeS << endl;
    }

    // stergere valoare
    static void deleteValue(string numeFisier, int nrColoanaConditieDelete, string conditieDelete)
    {
        // se recupereaza structura tabelului din fisierul binar
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        int startOffset = gestiuneFisiere::getStartOffset(numeFisier);
        int final = gestiuneFisiere::getFinal(numeFisier);
        // se redenumeste fisierul tabelei intr-un nume temporar
        string tempName = "~" + numeFisier;
        if (rename(numeFisier.data(), tempName.data()))
        {
            throw new runtime_error("Nu se poate crea fisierul temporar!");
        }
        // se deschide fisierul temporar (practic tabela veche, inainte de modificari)
        fstream f(tempName);
        // se creeaza un nou fisier pentru tabela
        gestiuneFisiere::createTable(t);
        string* values;
        f.seekp(startOffset);
        // se parcurge fisierul original al tabelei
        while (f.tellp() != final)
        {
            // se citesc valorile in string alocat dinamic (se parcurg linie cu linie, daca e sa fie privit ca un tabel)
            values = new string[t.getNrColoane()];
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                char* buffer;
                int length;
                f.read((char*)&length, sizeof(int));
                buffer = new char[length];
                f.read(buffer, length * sizeof(char));
                values[i] = buffer;
                delete[] buffer;
            }
            // daca conditia de delete nu este indeplinita, atunci valorile citite din fisierul tabelei se scriu in noul fisier al tabelei
            if (!(values[nrColoanaConditieDelete] == conditieDelete))
            {
                // pentru a reutiliza codul, daca valorile trebuie adaugate din nou, se converteste string* la un string cu spatii intre valori
                string allValues;
                for (int i = 0; i < t.getNrColoane(); i++)
                {
                    allValues.append(values[i]);
                    allValues.append(" ");
                }
                // pentru a putea utiliza functia mklista
                ListaCuvinte* valori = mklista(allValues);
                // pentru a putea folosi codul de mai sus care insereaza valorile
                gestiuneFisiere::addValues(numeFisier, valori);
            }
            delete[] values;
        }
        f.close();
        // se sterge vechiul fisier al tabelei
        remove(tempName.data());
    }

    // functia
    static void updateValues(string numeFisier, int nrColoanaSET, string valoareSET, int nrColoanaWHERE, string valoareWHERE)
    {
        // aproximativ aceleasi principii ca la delete
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        int startOffset = gestiuneFisiere::getStartOffset(numeFisier);
        int final = gestiuneFisiere::getFinal(numeFisier);
        string tempName = "~" + numeFisier;
        if (rename(numeFisier.data(), tempName.data()))
        {
            throw new runtime_error("Nu se poate crea fisierul temporar!");
        }
        fstream f(tempName);
        gestiuneFisiere::createTable(t);
        string* values;
        f.seekp(startOffset);
        while (f.tellp() != final)
        {
            values = new string[t.getNrColoane()];
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                char* buffer;
                int length;
                f.read((char*)&length, sizeof(int));
                buffer = new char[length];
                f.read(buffer, length * sizeof(char));
                values[i] = buffer;
                delete[] buffer;
            }
            // diferenta fiind ca daca se indeplineste conditia, atunci se schimba valoare in cea de dupa set
            if (values[nrColoanaWHERE] == valoareWHERE)
            {
                values[nrColoanaSET] = valoareSET;
            }
            string allValues;
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                allValues.append(values[i]);
                allValues.append(" ");
            }
            ListaCuvinte* valori = mklista(allValues);
            gestiuneFisiere::addValues(numeFisier, valori);
            delete[] values;
        }
        f.close();
        remove(tempName.data());
    }
};

int gestiuneFisiere::dpc = 0;
int gestiuneFisiere::sc = 0;

// comanda de baza, din care deriveaza toate
class cmdGENERIC
{
public:
    virtual int parse(ListaCuvinte* cmd)
    {
        return 1;
    }
    virtual void afisare()
    {
        cout << "\nComanda curenta: nerecunoscuta";
    }
};

// sintaxa:
// CREATE TABLE numeTabela IF NOT EXISTS
// + argumente (de adaugat)
class cmdCREATETABLE : public cmdGENERIC
{
    string numeTabela;
    bool ifNotExists = false;
    Tabela tabel;

    void copiere(int numeTabela)
    {
        this->numeTabela = numeTabela;
    }
public:
    cmdCREATETABLE()
    {
        // nu trebuie initializat initial
    }

    void afisare()
    {
        cout << "\nComanda curenta: CREATE TABLE";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant e "CREATE"
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "CREATE")
        {
            return 2;
        }
        cmd = cmd->next;

        // se verifica daca al doilea cuvant e "TABLE"
        if (cmd == NULL || cmd->cuvant != "TABLE")
        {
            throw new runtime_error("Comanda CREATE lipseste TABLE!");
        }
        cmd = cmd->next;

        // se verifica daca dupa CREATE TABLE exista si un nume de tabela
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda CREATE TABLE lipseste nume tabela!");
        }
        numeTabela = cmd->cuvant;

        // se verifica daca exista IF NOT EXISTS sau urmeaza coloane
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda CREATE TABLE lipsesc argumente dupa TABLE");
        }
        else
        {
            if (cmd->cuvant == "IF")
            {
                // IF NOT EXISTS
                cmd = cmd->next;
                if (cmd == NULL || cmd->cuvant != "NOT")
                {
                    throw new runtime_error("Comanda CREATE TABLE lipseste NOT EXISTS dupa IF");
                }

                cmd = cmd->next;
                if (cmd == NULL || cmd->cuvant != "EXISTS")
                {
                    throw new runtime_error("Comanda CREATE TABLE lipseste EXISTS dupa IF NOT");
                }

                cmd = cmd->next;
                if (cmd == NULL)
                {
                    throw new runtime_error("Comanda CREATE TABLE lipsesc argumente dupa IF NOT EXISTS");
                }

                ifNotExists = true;
            }

            // de aici se verifica (),(),()...
            if (cmd->cuvant == "(")
            {
                string argumenteTabela[8];
                for (int i = 0; i < 8; i++)
                {
                    cmd = cmd->next;
                    if (cmd == NULL)
                    {
                        throw new runtime_error("Comanda CREATE TABLE argumente introduse gresit in (...)");
                    }
                    else
                    {
                        argumenteTabela[i] = cmd->cuvant;
                    }
                }
                for (int i = 1; i < 6; i += 2)
                {
                    if (argumenteTabela[i] != ",")
                    {
                        throw new runtime_error("Comanda CREATE TABLE lipsesc virgule in (...)");
                    }
                }
                if (argumenteTabela[7] != ")")
                {
                    throw new runtime_error("Comanda CREATE TABLE lipeste paranteza ) in (...)");
                }
                else
                {
                    // [0] = nume
                    // [2] = tip (text, int, float)
                    // [4] = dim (<255)
                    // [6] = val_default (dim nu mai mare decat [4])
                    int tip;
                    if (argumenteTabela[2] != "text" && argumenteTabela[2] != "int" && argumenteTabela[2] != "float")
                    {
                        throw new runtime_error("Comanda CREATE TABLE, tip coloana necunoscut (tipuri cunoscute: text, int, float)");
                    }
                    else
                    {
                        if (argumenteTabela[2] == "text")
                        {
                            tip = 0;
                        }
                        if (argumenteTabela[2] == "int")
                        {
                            tip = 1;
                        }
                        if (argumenteTabela[2] == "float")
                        {
                            tip = 2;
                        }
                    }
                    if (argumenteTabela[4].size() > 3)
                    {
                        throw new runtime_error("Comanda CREATE TABLE, dimensiune coloana prea mare");
                    }
                    if (argumenteTabela[4].find_first_not_of("0123456789") != string::npos)
                    {
                        throw new runtime_error("Comanda CREATE TABLE, dimensiunea nu este un numar");
                    }
                    if (stoi(argumenteTabela[4]) == 0 || stoi(argumenteTabela[4]) > 255)
                    {
                        throw new runtime_error("Comanda CREATE TABLE, dimensiune coloana prea mare sau egala cu 0");
                    }
                    if (argumenteTabela[6].size() > stoi(argumenteTabela[4]))
                    {
                        throw new runtime_error("Comanda CREATE TABLE, dimensiune valoare predefinita mai mare decat dimensiune coloana!");
                    }

                    int dim = stoi(argumenteTabela[4]);
                    char* val;
                    val = new char[argumenteTabela[6].size() + 1];
                    val = (char*)argumenteTabela[6].data();
                    val[argumenteTabela[6].size()] = '\0';
                    Valoare v((Sql_TYPE)tip, dim, val);
                    Coloana c(argumenteTabela[0], (Sql_TYPE)tip, (int)stoi(argumenteTabela[4]), v);
                    tabel = tabel + c;
                }
            }
            else
            {
                throw new runtime_error("Comanda CREATE TABLE lipseste IF NOT EXISTS sau (coloana)");
            }
            cmd = cmd->next;
            while (cmd != NULL)
            {
                cmd = cmd->next;
                if (cmd == NULL)
                {
                    throw new runtime_error("Comanda CREATE TABLE nu exista nimic dupa virgula");
                }
                if (cmd->cuvant == "(")
                {
                    string argumenteTabela[8];
                    for (int i = 0; i < 8; i++)
                    {
                        cmd = cmd->next;
                        if (cmd == NULL)
                        {
                            throw new runtime_error("Comanda CREATE TABLE argumente introduse gresit in (...)");
                        }
                        else
                        {
                            argumenteTabela[i] = cmd->cuvant;
                        }
                    }
                    for (int i = 1; i < 6; i += 2)
                    {
                        if (argumenteTabela[i] != ",")
                        {
                            throw new runtime_error("Comanda CREATE TABLE lipsesc virgule in (...)");
                        }
                    }
                    if (argumenteTabela[7] != ")")
                    {
                        throw new runtime_error("Comanda CREATE TABLE lipeste paranteza ) in (...)");
                    }
                    else
                    {
                        int tip;
                        if (argumenteTabela[2] != "text" && argumenteTabela[2] != "int" && argumenteTabela[2] != "float")
                        {
                            throw new runtime_error("Comanda CREATE TABLE, tip coloana necunoscut (tipuri cunoscute: text, int, float)");
                        }
                        else
                        {
                            if (argumenteTabela[2] == "text")
                            {
                                tip = 0;
                            }
                            if (argumenteTabela[2] == "int")
                            {
                                tip = 1;
                            }
                            if (argumenteTabela[2] == "float")
                            {
                                tip = 2;
                            }
                        }
                        if (argumenteTabela[4].size() > 3)
                        {
                            throw new runtime_error("Comanda CREATE TABLE, dimensiune coloana prea mare");
                        }
                        if (argumenteTabela[4].find_first_not_of("0123456789") != string::npos)
                        {
                            throw new runtime_error("Comanda CREATE TABLE, dimensiunea nu este un numar");
                        }
                        if (stoi(argumenteTabela[4]) == 0 || stoi(argumenteTabela[4]) > 255)
                        {
                            throw new runtime_error("Comanda CREATE TABLE, dimensiune coloana prea mare sau egala cu 0");
                        }
                        if (argumenteTabela[6].size() > stoi(argumenteTabela[4]))
                        {
                            throw new runtime_error("Comanda CREATE TABLE, dimensiune valoare predefinita mai mare decat dimensiune coloana!");
                        }

                        int dim = stoi(argumenteTabela[4]);
                        char* val;
                        val = new char[argumenteTabela[6].size()];
                        val = (char*)argumenteTabela[6].data();
                        Valoare v((Sql_TYPE)tip, dim, val);
                        Coloana c(argumenteTabela[0], (Sql_TYPE)tip, (int)stoi(argumenteTabela[4]), v);
                        cmd = cmd->next;
                        tabel = tabel + c;
                    }
                }
                else
                {
                    throw new runtime_error("Comanda CREATE TABLE lipsesc (coloane) dupa ,");
                }
            }

            tabel.setNumeTabela(numeTabela);
            afisareCreateTable();
            executaCreateTable();
            return 0;
        }
    }

    void afisareCreateTable()
    {
        cout << endl << "comanda CREATE TABLE" << endl;
        cout << "  nume tabela : " << numeTabela << endl;
        cout << "  [IF NOT EXISTS]: ";
        if (ifNotExists)
        {
            cout << "true";
        }
        else
        {
            cout << "false";
        }
        cout << tabel;
    }

    void executaCreateTable()
    {
        // ce se scrie in fisier:
        // nume tabela + ifnotexists + tabel(nrcoloane + coloana(denumire, tip, dim, val(adim, text))
        if (ifNotExists)
        {
            fstream test(tabel.getNumeTabela() + ".bin");
            if (test.is_open())
            {
                test.close();
                throw new runtime_error("Comanda CREATE TABLE, exista deja un tabel cu acest nume!");
            }
            else
            {
                gestiuneFisiere::createTable(this->tabel);
                cout << "\nTabel creat cu succes!\n";
            }
        }
        else
        {
            gestiuneFisiere::createTable(this->tabel);
            cout << "\nTabel cret cu succes!\n";
        }
    }

    ~cmdCREATETABLE()
    {
        // nimic de sters
    }
};

// sintaxa:
// DROP TABLE numeTabela
class cmdDROPTABLE : public cmdGENERIC
{
    string numeTabela;

    void copiere(int numeTabela)
    {
        this->numeTabela = numeTabela;
    }
public:
    cmdDROPTABLE()
    {
        // nu trebuie initializat initial
    }

    void afisare()
    {
        cout << "\nComanda curenta: DROP TABLE";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant e "DROP"
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "DROP")
        {
            return 2;
        }
        cmd = cmd->next;

        // se verifica daca al doilea cuvant e "TABLE"
        if (cmd == NULL || cmd->cuvant != "TABLE")
        {
            throw new runtime_error("Comanda DROP lipseste TABLE!");
        }
        cmd = cmd->next;

        // se verifica daca dupa "DROP TABLE" mai exista un nume de coloana
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda DROP TABLE lipseste nume tabela!");
        }
        numeTabela = cmd->cuvant;

        afisareDropTable();
        executaDropTable();
        return 0;
    }

    void afisareDropTable()
    {
        cout << endl << "comanda DROP TABLE" << endl;
        cout << "  nume tabela : " << numeTabela << endl;
    }

    void executaDropTable()
    {
        string numeFisier = numeTabela;
        numeFisier.append(".bin");
        if (gestiuneFisiere::existaFisier(numeFisier))
        {
            remove(numeFisier.data());
            cout << "Tabel sters cu succes!\n";
        }
        else
        {
            throw new runtime_error("Comanda DROP TABLE, tabel inexistent");
        }
    }

    ~cmdDROPTABLE()
    {
        // nimic de steres
    }
};

// sintaxa:
// DISPLAY TABLE numeTabela
class cmdDISPLAYTABLE : public cmdGENERIC
{
    string numeTabela;

    void copiere(int numeTabela)
    {
        this->numeTabela = numeTabela;
    }
public:
    cmdDISPLAYTABLE()
    {
        // nu trebuie initializat initial
    }

    void afisare()
    {
        cout << "\nComanda curenta: DISPLAY TABLE";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant e "DISPLAY"
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "DISPLAY")
        {
            return 2;
        }

        // se verifica daca al doilea cuvant e "TABLE"
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "TABLE")
        {
            throw new runtime_error("Comanda DISPLAY TABLE lipseste TABLE!");
        }

        // se verifica daca dupa "DISPLAY TABLE" mai apare si un nume de tabela
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda DISPLAY TABLE lipseste nume tabela!");
        }
        numeTabela = cmd->cuvant;


        afisareDisplayTable();
        executaDisplayTable();
        return 0;
    }

    void afisareDisplayTable()
    {
        cout << endl << "comanda DISPLAY TABLE" << endl;
        cout << "  nume tabela : " << numeTabela << endl << endl;
    }

    void executaDisplayTable()
    {
        string numeFisier = numeTabela;
        numeFisier.append(".bin");
        if (gestiuneFisiere::existaFisier(numeFisier))
        {
            gestiuneFisiere::displayTableConsola(numeFisier);
            gestiuneFisiere::displayTableFisier(numeFisier);
        }
        else
        {
            throw new runtime_error("Comanda DISPLAY TABLE, tabel inexistent");
        }
    }

    ~cmdDISPLAYTABLE()
    {
        // nimic de sters
    }
};

// sintaxa:
// INSERT INTO numeTabela VALUES
// + argumente
class cmdINSERT : public cmdGENERIC
{
    string numeTabela;
    ListaCuvinte* values = NULL;
    bool import = false;
public:
    cmdINSERT()
    {
        // nu trebuie initializat neaparat
    }

    void afisare()
    {
        cout << "\nComanda curenta: INSERT";
    }

    // constructor special pt comanda import, apelat doar de ea
    cmdINSERT(bool import)
    {
        this->import = import;
    }

    int parse(ListaCuvinte* cmd)
    {
        ListaCuvinte* r;


        // se verifica daca primul cuvant este "INSERT"
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "INSERT")
        {
            return 2;
        }

        // se verifica daca al doilea cuvant este "INTO"
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "INTO")
        {
            throw new runtime_error("Comanda INSERT lipseste INTO!");
        }
        // se verifica daca dupa INSERT INTO exista un nume de tabela
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda INSERT INTO lipseste nume tabela!");
        }
        numeTabela = cmd->cuvant;

        // se verifica daca dupa INSERT INTO numeTabela exista VALUES
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "VALUES")
        {
            throw new runtime_error("Comanda INSERT lipseste VALUES dupa tabela!");
        }

        // se verifica daca urmeaza (
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "(")
        {
            throw new runtime_error("Comanda INSERT lipseste (...) dupa VALUES");
        }
        else
        {
            // se face o noua lista de cuvinte pentru VALUES
            cmd = cmd->next;
            values = new ListaCuvinte(cmd->cuvant);
            r = values;

            do
            {
                cmd = cmd->next;
                if (cmd == NULL)
                {
                    throw new runtime_error("Comanda INSERT lipsc argumentele, virgula sau parantezele (...)");
                }

                // end loop daca se gaseste )
                if (cmd->cuvant == ")")
                {
                    break;
                }

                if (cmd->cuvant != ",")
                {
                    throw new runtime_error("Comanda INSERT lipseste virgula intre argumente");
                }

                cmd = cmd->next;
                if (cmd == NULL)
                {
                    throw new runtime_error("Comanda INSERT lipseste valoare dupa virgula!");
                }

                r->next = new ListaCuvinte(cmd->cuvant);
                r = r->next;
            } while (cmd);
        }

        if (!values)
        {
            throw new runtime_error("Comanda INSERT nu au fost introduse argumente in (..)");
        }

        afisareINSERT();
        executaINSERT(import);
        return 0;
    }

    void afisareINSERT()
    {
        cout << endl << "comanda INSERT INTO" << endl;
        cout << "  nume tabela : " << numeTabela << endl;
        cout << "  lista valori : ";
        cout << *values << endl;
    }

    // flag import, daca e true se afiseaza erorile cu IMPORT in loc de INSERT
    void executaINSERT(bool import)
    {
        // se citeste tabela din fisier
        Tabela t = gestiuneFisiere::retrieveTable(numeTabela + ".bin");
        // verificare daca e acelasi numar de valori
        ListaCuvinte* copieValues = values;
        int count = 0;
        while (copieValues != NULL)
        {
            count++;
            copieValues = copieValues->next;
        }
        if (count != t.getNrColoane())
        {
            throw new runtime_error("Tabelul are un numar diferit de coloane decat numarul de valori introduse");
        }
        // verificare tip valori
        bool* ok = new bool[count];
        copieValues = values;
        for (int i = 0; i < count; i++)
        {
            ok[i] = false;
            int tipActual = t.getColoana(i)->getTip(); // se recupereaza tipul valorii din coloana
            bool numar = true; // daca este numar, poate fi introdus la int, float, text
            bool posibilFloat = false; // daca este float, poate fi doar float, text
            bool text = false; // daca este text, poate fi doar text

            for (int j = 0; j < copieValues->cuvant.length(); j++)
            {
                if (copieValues->cuvant[j] == '.')
                {
                    posibilFloat = true;
                    numar = false;
                }
                if ((copieValues->cuvant[j] >= 'A' && copieValues->cuvant[j] <= 'Z') || (copieValues->cuvant[j] >= 'a' && copieValues->cuvant[j] <= 'z'))
                {
                    text = true;
                    numar = false;
                }
            }
            if (numar) // numarul se potriveste la toate 3 tipuri
            {
                ok[i] = true;
            }
            if (posibilFloat) // daca contine . e ori float ori text
            {
                if (tipActual == Sql_TYPE::Sql_INT)
                {
                    if (!import)
                    {
                        throw new runtime_error("Comanda INSERT, tipul valorilor din (...) nu se potriveste cu tipul valorilor din coloanele tabelului");
                    }
                    else
                    {
                        throw new runtime_error("Comanda IMPORT, tipul valorilor din (...) nu se potriveste cu tipul valorilor din coloanele tabelului");
                    }
                }
                ok[i] = true;
            }
            if (text) // daca e text, poate sa fie doar text
            {
                if (tipActual != Sql_TYPE::Sql_TEXT)
                {
                    if (!import)
                    {
                        throw new runtime_error("Comanda INSERT, tipul valorilor din (...) nu se potriveste cu tipul valorilor din coloanele tabelului");
                    }
                    else
                    {
                        throw new runtime_error("Comanda IMPORT, tipul valorilor din (...) nu se potriveste cu tipul valorilor din coloanele tabelului");
                    }
                }
                ok[i] = true;
            }
            copieValues = copieValues->next;
        }
        for (int i = 0; i < count; i++)
        {
            if (ok[i] == false)
            {
                if (!import)
                {
                    throw new runtime_error("Comanda INSERT, tipul valorilor din (...) nu se potriveste cu tipul valorilor din coloanele tabelului");
                }
                else
                {
                    throw new runtime_error("Comanda IMPORT, tipul valorilor din (...) nu se potriveste cu tipul valorilor din coloanele tabelului");
                }
            }
        }
        delete[] ok;
        // verificare dimensiune valoare
        copieValues = values;
        for (int i = 0; i < count; i++)
        {
            int dim = t.getColoana(i)->getDim();
            if (dim < copieValues->cuvant.length())
            {
                if (!import)
                {
                    throw new runtime_error("Comanda INSERT, dimensiunea valorilor din (...) mai mare decat dimensiunea maxima");
                }
                else
                {
                    throw new runtime_error("Comanda IMPORT, dimensiunea valorilor din (...) mai mare decat dimensiunea maxima");
                }
            }
            copieValues = copieValues->next;
        }
        // totul ok, se poate scrie in fisier
        gestiuneFisiere::addValues(numeTabela + ".bin", values);
        if (!import)
        {
            cout << "\nValorile au fost inserate in tabela cu succes!\n";
        }
    }

    ~cmdINSERT()
    {
        // nimic de sters
    }
};

// sintaxa:
// DELETE FROM numeTabela WHERE numeColoana = strval
class cmdDELETE : public cmdGENERIC
{
    string numeTabela;
    string numeColoana;
    string strval;

    Valoare val;

    void copiere(int numeTabela)
    {
        this->numeTabela = numeTabela;
    }
public:
    cmdDELETE()
    {
        // nu trebuie initializat neaparat
    }

    void afisare()
    {
        cout << "\nComanda curenta: DELETE";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant este DELETE
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "DELETE")
        {
            return 2;
        }

        // se verifica daca al doilea cuvant este FROM
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "FROM")
        {
            throw new runtime_error("Comanda DELETE lipseste FROM!");
        }

        // se verifica daca dupa DELETE FROM urmeaza un nume de tabela
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda DELETE FROM lipseste nume tabela!");
        }
        numeTabela = cmd->cuvant;

        // se verifica daca dupa DELETE FROM numeTabela urmeaza WHERE
        cmd = cmd->next;
        if ((cmd == NULL) || (cmd->cuvant != "WHERE"))
        {
            throw new runtime_error("Comanda DELETE FROM lipseste WHERE !");
        }

        // se verifica daca dupa DELETE FROM numeTabela WHERE urmeaza numeColoana
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda DELETE FROM lipseste nume coloana!");
        }
        numeColoana = cmd->cuvant;

        // se verifica daca dupa DELETE FROM numeTabela WHERE numeColoana urmeaza =
        cmd = cmd->next;
        if ((cmd == NULL) || (cmd->cuvant != "="))
        {
            throw new runtime_error("Comanda DELETE FROM lipseste = !");
        }

        // se verifica daca dupa DELETE FROM numeTabela WHERE numeColoana= urmeaza strval
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda DELETE FROM lipseste valoare!");
        }
        strval = cmd->cuvant;

        afisareDelete();
        executaDelete();
        return 0;
    }

    void afisareDelete()
    {
        cout << endl << "comanda DELETE FROM" << endl;
        cout << "  nume tabela : " << numeTabela << endl;
        cout << "  nume coloana : " << numeColoana << endl;
        cout << "  valoare : " << strval << endl;
    }

    void executaDelete()
    {
        string numeFisier = numeTabela + ".bin";
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        bool coloanaOK = false;
        int nrColoana;
        // verificare nume coloana
        for (int i = 0; i < t.getNrColoane(); i++)
        {
            if (numeColoana == t.getColoana(i)->getDenumire())
            {
                // daca se gaseste, se ia si numarul ei pentru a putea face modificarile
                nrColoana = i;
                coloanaOK = true;
            }
        }
        if (!coloanaOK)
        {
            throw new runtime_error("Comanda DELETE, in tabel nu exista coloana cu aceasta denumire!");
        }
        // totul ok
        gestiuneFisiere::deleteValue(numeFisier, nrColoana, strval);
        cout << "\nValori eliminate cu succes!\n";
    }

    ~cmdDELETE()
    {
        // nimic de sters
    }
};

// sintaxa:
// SELECT listancol / ALL FROM numeTabela WHERE numeColoana = strval
class cmdSELECT : public cmdGENERIC
{
    string numeTabela;
    string numeColoana;
    string strval;
    ListaCuvinte* listancol = NULL;
    // == NULL daca e ALL lista nume coloane
    bool where = false;
    bool all = false;

    void copiere(int numeTabela)
    {
        this->numeTabela = numeTabela;
    }
public:
    cmdSELECT()
    {
        // nu trebuie neaparat initializat momentan
    }

    void afisare()
    {
        cout << "\nComanda curenta: SELECT";
    }

    int parse(ListaCuvinte* cmd)
    {
        ListaCuvinte* r;

        // se verifica daca primul cuvant este "SELECT"
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "SELECT")
        {
            return 2;
        }

        // se verifica daca dupa SELECT urmeaza ceva
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda SELECT lipseste nume coloana!");
        }

        // se verifica daca dupa SELECT urmeaza ALL FROM
        if (cmd->cuvant == "ALL")
        {
            all = true;
            listancol = NULL;
            cmd = cmd->next;
            if (cmd == NULL || cmd->cuvant != "FROM")
            {
                throw new runtime_error("Comanda SELECT lipseste FROM dupa ALL!");
            }
        }
        else
        {
            // parse lista coloane()
            if (cmd->cuvant == "FROM")
            {
                // cazul SELECT FROM
                throw new runtime_error("Comanda SELECT are nevoie de cel putin o coloana inainte de FROM!");
            }

            // listancol contine lista coloanelor
            listancol = new ListaCuvinte(cmd->cuvant);
            r = listancol;
            do
            {
                cmd = cmd->next;
                if (cmd == NULL)
                {
                    throw new runtime_error("Comanda SELECT lipseste FROM dupa lista coloane!");
                }

                // end loop daca se gaseste FROM
                if (cmd->cuvant == "FROM")
                {
                    break;
                }

                if (cmd->cuvant != ",")
                {
                    throw new runtime_error("Comanda SELECT lipseste virgula sau FROM dupa nume coloana!");
                }

                cmd = cmd->next;
                if (cmd == NULL)
                {
                    throw new runtime_error("Comanda SELECT lipseste nume coloana dupa virgula!");
                }

                r->next = new ListaCuvinte(cmd->cuvant);
                r = r->next;
            } while (cmd);
        }

        // se verifica daca exista numeTabela dupa FROM
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda SELECT lipseste nume tabela dupa FROM!");
        }
        numeTabela = cmd->cuvant;

        // se verifica daca exista WHERE
        cmd = cmd->next;
        if (cmd == NULL)
        {
            where = false;
        }
        else
        {
            if (cmd->cuvant != "WHERE")
            {
                throw new runtime_error("Comanda SELECT lipseste WHERE dupa nume coloana");
            }
            where = true;
            // se verifica daca dupa WHERE exista numeColoana
            cmd = cmd->next;
            if (cmd == NULL)
            {
                throw new runtime_error("Comanda SELECT lipseste nume coloana dupa WHERE");
            }
            this->numeColoana = cmd->cuvant;

            // se verifica daca exista = dupa numeColoana
            cmd = cmd->next;
            if ((cmd == NULL) || (cmd->cuvant != "="))
            {
                throw new runtime_error("Comanda SELECT lipseste = dupa WHERE!");
            }

            // se verifica daca dupa = urmeaza strval
            cmd = cmd->next;
            if (cmd == NULL)
            {
                throw new runtime_error("Comanda SELECT lipseste valoare! dupa =");
            }

            // totul ok, se baga valoarea
            this->strval = cmd->cuvant;
        }
        afisareSelect();
        executaSelect();
        return 0;
    }

    void afisareSelect()
    {
        cout << endl << "comanda SELECT" << endl;
        cout << "  lista coloane : ";
        if (listancol)
        {
            cout << *listancol << endl;
        }
        else
        {
            cout << "ALL" << endl;
        }
        cout << "  nume tabela : " << numeTabela << endl;
        if (where)
        {
            cout << "  WHERE" << endl;
            cout << "  nume coloana : " << numeColoana << endl;
            cout << "  valoare : " << strval << endl;
        }
    }

    void executaSelect()
    {
        ListaCuvinte* copie = listancol;
        int nrColoane = 0;
        string* denCol;
        string numeFisier = numeTabela + ".bin";
        // se preia tabela din fisier
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        // se numara coloanele si se creeaza o copie locala a numelor acestora (pentru acces mai usor la date)
        if (!all)
        {
            // daca nu sunt toate se parcurge lista pentru a afla numarul
            while (copie != NULL)
            {
                nrColoane++;
                copie = copie->next;
            }
            denCol = new string[nrColoane];
            copie = listancol;
            int count = 0;
            // si se mai parcurge inca o data pentru a afla denumirile
            while (copie != NULL)
            {
                denCol[count] = copie->cuvant;
                copie = copie->next;
                count++;
            }
        }
        else
        {
            // daca sunt toate se extrag direct din tabel
            nrColoane = t.getNrColoane();
            denCol = new string[nrColoane];
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                denCol[i] = t.getColoana(i)->getDenumire();
            }
        }
        // se verifica daca numarul de coloane introdus este ok
        if (t.getNrColoane() < nrColoane)
        {
            throw new runtime_error("Comanda SELECT, se incearca selectarea mai multor coloane decat exista in sistem!");
        }
        // se verifica daca numele coloanelor introduse este ok si se stocheaza ordinea coloanelor cerute pentru a afisa in ordine
        int* ordineColoaneSelect = new int[nrColoane];
        bool* denumireOK = new bool[nrColoane];
        for (int i = 0; i < nrColoane; i++)
        {
            // initial presupunem ca numele nu sunt introduse bine
            denumireOK[i] = false;
        }
        for (int i = 0; i < nrColoane; i++)
        {
            // pentru fiecare coloana in parte
            for (int j = 0; j < t.getNrColoane(); j++)
            {
                // se verifica toate denumirile posibile
                if (t.getColoana(j)->getDenumire() == denCol[i])
                {
                    // daca exista, atunci se schimba pe true, insemnand ca acea coloana exista
                    ordineColoaneSelect[i] = j;
                    denumireOK[i] = true;
                }
            }
        }
        // se verifica daca toate coloanele sunt ok
        for (int i = 0; i < nrColoane; i++)
        {
            if (denumireOK[i] == false)
            {
                throw new runtime_error("Comanda SELECT, nu exista coloana care se incearca a fi selectata!");
            }
        }
        delete[] denumireOK;
        // se verifica daca coloana din where exista
        bool coloanaWhereOK = false;
        int nrColoanaConditieWhere = -1;
        if (where)
        {
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                if (t.getColoana(i)->getDenumire() == numeColoana)
                {
                    // daca exista, se retine si numarul ei pentru a fi utilizat mai incolo
                    nrColoanaConditieWhere = i;
                    coloanaWhereOK = true;
                }
            }
            if (!coloanaWhereOK)
            {
                throw new runtime_error("Comanda SELECT, coloana din WHERE nu exista in tabel!");
            }
        }
        // totul ok, se executa
        gestiuneFisiere::select(numeFisier, nrColoane, ordineColoaneSelect, where, nrColoanaConditieWhere, strval);
        gestiuneFisiere::selectFisier(numeFisier, nrColoane, ordineColoaneSelect, where, nrColoanaConditieWhere, strval);
        delete[] ordineColoaneSelect;
    }

    ~cmdSELECT()
    {
        // nimic de sters
    }
};

// sintaxa: IMPORT numeTabela numeFisier.csv
class cmdIMPORT : public cmdGENERIC
{
    string numeTabela;
    string numeCSV;
public:
    cmdIMPORT()
    {
        // nimic de initializat
    }

    void afisare()
    {
        cout << "\nComanda curenta: IMPORT";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant e IMPORT
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "IMPORT")
        {
            return 2;
        }

        // se verifica daca exista numeTabela
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda IMPORT, lipseste numeTabela");
        }
        numeTabela = cmd->cuvant;

        // se verifica daca exista numeCSV
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda IMPORT, lipseste numeCSV");
        }
        // daca ultimele 4 caractere din cel de-al doilea parametru nu sunt egale cu .csv inseamna ca nu e un fisier csv (logic)
        if (cmd->cuvant.substr(cmd->cuvant.length() - 4) != ".csv")
        {
            throw new runtime_error("Comanda IMPORT, fisierul trimis nu este .csv");
        }
        numeCSV = cmd->cuvant;

        afiseazaIMPORT();
        executaIMPORT();
        return 0;
    }

    void afiseazaIMPORT()
    {
        cout << endl << "comanda IMPORT" << endl;
        cout << "  nume tabela : " << numeTabela << endl;
        cout << "  nume fisier csv : " << numeCSV << endl;
    }

    void executaIMPORT()
    {
        string numeFisier = numeTabela + ".bin";
        // se recupereaza tabela din fisierul binar asociat
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        // se incearca deschiderea fisierului csv
        fstream f(numeCSV);
        if (!f.is_open())
        {
            throw new runtime_error("Comanda IMPORT, eroare la deschiderea fisierului CSV");
        }
        int nrValori = 0;
        // file e un string ce contine toate valorile din fisierul csv
        string file;
        // temp e un string ce contine un rand din fisierul csv
        string temp;
        while (!f.eof())
        {
            f >> temp;
            file.append(temp);
        }
        // se reutilizeaza functia mklista din stringul file
        ListaCuvinte* values = mklista(file);
        // se creeaza o copie pentru a numara valorile
        ListaCuvinte* copie = values;
        while (copie != NULL)
        {
            string value;
            string virgula;
            value = copie->cuvant;
            nrValori++;
            copie = copie->next;
            if (copie == NULL)
            {
                break;
            }
            else
            {
                virgula = copie->cuvant;
                if (virgula != ",")
                {
                    throw new runtime_error("Comanda IMPORT, lipsesc virgule in fisierul CSV!");
                }
            }
            copie = copie->next;
        }
        // daca nr valori nu e divizibil cu nr de coloane inseamna ca ar ramane coloane necompletate (not ok)
        if (nrValori % t.getNrColoane())
        {
            throw new runtime_error("Comanda IMPORT, numar valorilor din CSV este unul imposibil de introdus in tabela (deoarece ar ramane coloane goale)");
        }
        // se parcurg coloanele
        while (values != NULL)
        {
            // se citesc cate t.getNrColoane() valori odata si se creeaza o comanda de tip INSERT
            string cmdINSERTl = "INSERT INTO " + numeTabela + " VALUES (";
            for (int i = 0; i < t.getNrColoane(); i++)
            {
                cmdINSERTl.append(values->cuvant);
                if (i != t.getNrColoane() - 1)
                {
                    cmdINSERTl.append(",");
                }
                if (values != NULL)
                {
                    values = values->next; // se trece la virgula
                }
                if (values != NULL)
                {
                    values = values->next; // se trece la urmatoarea valoare
                }
            }
            cmdINSERTl.append(")");
            // se creeaza o noua lista de cuvinte pentru a stoca valorile din comanda insert (necesar pt parsare)
            ListaCuvinte* cmdINSERTlc = mklista(cmdINSERTl);
            // se apeleaza constructorul special pt import al comenzii insert
            cmdINSERT parser(true);
            // se proceseaza comanda de tip insert (deoarece acolo sunt deja toate validarile pentru tipuri gata facute si e mai simplu asa decat sa fie regandite)
            parser.parse(cmdINSERTlc);
        }
        cout << "\nValorile au fost importate cu succes din fisierul .CSV!\n";
    }

    ~cmdIMPORT()
    {
        // nimic de sters
    }
};

// sintaxa:
// UPDATE numeTabela SET numeColoana=strval WHERE numeColoanaConditie=strvalConditie
class cmdUPDATE : public cmdGENERIC
{
    string numeTabela;
    string numeColoana;
    string strval;
    string numeColoanaConditie;
    string strvalConditie;

    void copiere(int numeTabela)
    {
        this->numeTabela = numeTabela;
    }
public:
    cmdUPDATE()
    {
        // nu trebuie initializat neaparat
    }

    void afisare()
    {
        cout << "\nComanda curenta: UPDATE";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant este UPDATE
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "UPDATE")
        {
            return 2;
        }

        // se verifica daca dupa UPDATE urmeaza numeTabela
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda UPDATE lipseste nume tabela!");
        }
        numeTabela = cmd->cuvant;

        // se verifica daca dupa UPDATE numeTabela urmeaza SET
        cmd = cmd->next;
        if ((cmd == NULL) || (cmd->cuvant != "SET"))
        {
            throw new runtime_error("Comanda UPDATE lipseste SET!");
        }

        // se verifica daca dupa UPDATE numeTabela SET urmeaza numeColoana
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda UPDATE lipseste nume coloana!");
        }
        numeColoana = cmd->cuvant;

        // se verifica daca dupa UPDATE numeTabela SET numeColoana urmeaza =
        cmd = cmd->next;
        if ((cmd == NULL) || (cmd->cuvant != "="))
        {
            throw new runtime_error("Comanda UPDATE lipseste = !");
        }

        // se verifica daca dupa UPDATE numeTabela SET numeColoana= urmeaza strval
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda UPDATE lipseste valoare!");
        }
        strval = cmd->cuvant;

        // se verifica daca dupa strval urmeaza WHERE
        cmd = cmd->next;
        if (cmd == NULL || (cmd->cuvant != "WHERE"))
        {
            throw new runtime_error("Comanda UPDATE, lipseste cuvant cheie WHERE");
        }

        // se verifica daca dupa WHERE exista numeColoana
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda UPDATE, lipseste numele coloanei dupa WHERE");
        }
        numeColoanaConditie = cmd->cuvant;

        // se verifica daca dupa numeColoana exista =
        cmd = cmd->next;
        if ((cmd == NULL) || (cmd->cuvant != "="))
        {
            throw new runtime_error("Comanda UPDATE lipseste = !");
        }

        // se verifica daca exista o valoare dupa egal
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda UPDATE, lipseste strval!");
        }
        strvalConditie = cmd->cuvant;

        afiseazaUPDATE();
        executaUPDATE();
        return 0;
    }

    void afiseazaUPDATE()
    {
        cout << endl << "comanda UPDATE" << endl;
        cout << "  nume tabela : " << numeTabela << endl;
        cout << "  SET" << endl;
        cout << "  nume coloana : " << numeColoana << endl;
        cout << "  valoare : " << strval << endl;
        cout << "  WHERE" << endl;
        cout << "  nume coloana conditie : " << numeColoanaConditie << endl;
        cout << "  valoare conditie : " << strvalConditie << endl;
    }

    void executaUPDATE()
    {
        string numeFisier = numeTabela + ".bin";
        // se recupereaza structura tabelei din fisierul asociat
        Tabela t = gestiuneFisiere::retrieveTable(numeFisier);
        // bool ce reprezinta daca coloanele alea exista
        bool coloanaSET = false;
        bool coloanaWHERE = false;
        // int ce repretzinta nr coloanelor (daca exista)
        int nrColoanaSET;
        int nrColoanaWHERE;
        // se verifica existenta coloanelor
        for (int i = 0; i < t.getNrColoane(); i++)
        {
            if (t.getColoana(i)->getDenumire() == numeColoana)
            {
                coloanaSET = true;
                nrColoanaSET = i;
            }
            if (t.getColoana(i)->getDenumire() == numeColoanaConditie)
            {
                coloanaWHERE = true;
                nrColoanaWHERE = i;
            }
        }
        // se trimit mesajele de eroare daca e cazul
        if (!coloanaSET)
        {
            throw new runtime_error("Comanda UPDATE, coloana dupa SET nu exista in tabel!");
        }
        if (!coloanaWHERE)
        {
            throw new runtime_error("Comanda UPDATE, coloana dupa WHERE nu exista in tabel!");
        }
        // se executa comanda
        gestiuneFisiere::updateValues(numeFisier, nrColoanaSET, strval, nrColoanaWHERE, strvalConditie);
    }

    ~cmdUPDATE()
    {
        // nimic de sters
    }
};

// sintaxa:
// CREATE INDEX IF NOT EXISTS nume_index ON nume_tabela(nume_coloana)
// nefunctional pt faza 2 (functional doar la nivelul fazei 1 deocamdata)
class cmdCREATEINDEX : public cmdGENERIC
{
    string numeIndex;
    string numeTabela;
    string numeColoana;
    bool ifNotExists = false;
public:
    void afisare()
    {
        cout << "\nComanda curenta: CREATE INDEX";
    }
    int parse(ListaCuvinte* cmd)
    {
        // primul cuvant sa fie CREATE
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "CREATE")
        {
            return 2;
        }
        cmd = cmd->next;
        // al doilea cuvant sa fie INDEX
        if (cmd == NULL || cmd->cuvant != "INDEX")
        {
            throw new runtime_error("Comanda CREATE INDEX lipseste INDEX");
        }
        // procesare al treilea cuvant
        cmd = cmd->next;
        if (cmd == NULL)
        {
            // nu e nimic
            throw new runtime_error("Comanda CREATE INDEX lipseste numeIndex");
        }
        else if (cmd->cuvant == "IF")
        {
            // IF NOT EXISTS
            cmd = cmd->next;
            if (cmd == NULL || cmd->cuvant != "NOT")
            {
                throw new runtime_error("Comanda CREATE INDEX lipseste NOT din IF NOT EXISTS");
            }
            cmd = cmd->next;
            if (cmd == NULL || cmd->cuvant != "EXISTS")
            {
                throw new runtime_error("Comanda CREATE INDEX lipseste EXISTS din IF NOT EXISTS");
            }
            cmd = cmd->next;
            if (cmd == NULL)
            {
                throw new runtime_error("Comanda CREATE INDEX lipseste numeIndex dupa IF NOT EXISTS");
            }
            ifNotExists = true;
            numeIndex = cmd->cuvant;
        }
        else
        {
            // este numeIndex
            numeIndex = cmd->cuvant;
        }
        // urmatorul cuvant sa fie ON
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "ON")
        {
            throw new runtime_error("Comanda CREATE INDEX lipseste ON dupa numeIndex");
        }
        // urmatorul cuvant sa fie numeTabela
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda CREATE INDEX lipseste numeTabela dupa ON");
        }
        numeTabela = cmd->cuvant;
        // urmatorul cuvant sa fie deschiderea parantezei
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != "(")
        {
            throw new runtime_error("Comanda CREATE INDEX lipseste (...) dupa numeTabela");
        }
        // urmatorul cuvant sa fie numeColoana
        cmd = cmd->next;
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda CREATE INDEX lipseste numeColoana din (...)");
        }
        numeColoana = cmd->cuvant;
        // urmatorul cuvant sa fie inchiderea parantezei
        cmd = cmd->next;
        if (cmd == NULL || cmd->cuvant != ")")
        {
            throw new runtime_error("Comanda CREATE INDEX lipseste ')' din '(...)'");
        }

        // totul ok, se afiseaza
        cout << endl << "comanda CREATE INDEX" << endl;
        cout << "  [IF NOT EXISTS]: ";
        if (ifNotExists)
        {
            cout << "true";
        }
        else
        {
            cout << "false";
        }
        cout << endl;
        cout << "  numeIndex: " << numeIndex << endl;
        cout << "  numeTabela: " << numeTabela << endl;
        cout << "  numeColoana: " << numeColoana << endl;
        return 0;
    }
};

// sintaxa:
// DROP INDEX nume_index
// nefunctional pt faza 2 (functional doar la nivelul fazei 1 deocamdata)
class cmdDROPINDEX : public cmdGENERIC
{
    string numeIndex;

    void copiere(int numeIndex)
    {
        this->numeIndex = numeIndex;
    }
public:
    cmdDROPINDEX()
    {
        // nu trebuie initializat initial
    }

    void afisare()
    {
        cout << "\nComanda curenta: DROP INDEX";
    }

    int parse(ListaCuvinte* cmd)
    {
        // se verifica daca primul cuvant e "DROP"
        if (cmd == NULL)
        {
            return 1;
        }
        if (cmd->cuvant != "DROP")
        {
            return 2;
        }
        cmd = cmd->next;

        // se verifica daca al doilea cuvant e "INDEX"
        if (cmd == NULL || cmd->cuvant != "INDEX")
        {
            throw new runtime_error("Comanda DROP INDEX lipseste INDEX!");
        }
        cmd = cmd->next;

        // se verifica daca dupa "DROP INDEX" mai exista un nume de coloana
        if (cmd == NULL)
        {
            throw new runtime_error("Comanda DROP INDEX lipseste nume index!");
        }

        // totul ok, se afiseaza
        numeIndex = cmd->cuvant;
        cout << endl << "comanda DROP INDEX" << endl;
        cout << "  nume index : " << numeIndex << endl << endl;
        return 0;
    }

    ~cmdDROPINDEX()
    {
        // nimic de steres
    }
};

// create x verifica daca e create table sau create index
class cmdCREATEx : public cmdGENERIC
{
public:
    void afisare()
    {
        cout << "\nComanda curenta: CREATE X";
    }
    int parse(ListaCuvinte* cmd)
    {
        ListaCuvinte* copieCMD = cmd;
        cmdGENERIC* cmdCREATE[2];
        cmdCREATE[0] = new cmdCREATETABLE();
        cmdCREATE[1] = new cmdCREATEINDEX();
        if (copieCMD == NULL)
        {
            return 1;
        }
        if (copieCMD->cuvant == "CREATE")
        {
            copieCMD = copieCMD->next;
            if (copieCMD == NULL)
            {
                throw new runtime_error("Comanda CREATE sintaxa nerecunoscuta!");
            }
            if (copieCMD->cuvant == "TABLE")
            {
                // parser table
                return cmdCREATE[0]->parse(cmd);
            }
            else if (copieCMD->cuvant == "INDEX")
            {
                // parser index
                return cmdCREATE[1]->parse(cmd);
            }
            else
            {
                throw new runtime_error("Comanda CREATE sintaxa nerecunoscuta!");
            }
        }
        else
        {
            return 2;
        }
    }
};

// drop x verifica daca e drop table sau drop index
class cmdDROPx : public cmdGENERIC
{
public:
    void afisare()
    {
        cout << "\nComanda curenta: DROP X";
    }
    int parse(ListaCuvinte* cmd)
    {
        ListaCuvinte* copieCMD = cmd;
        cmdGENERIC* cmdDROP[2];
        cmdDROP[0] = new cmdDROPTABLE();
        cmdDROP[1] = new cmdDROPINDEX();
        if (copieCMD == NULL)
        {
            return 1;
        }
        if (copieCMD->cuvant == "DROP")
        {
            copieCMD = copieCMD->next;
            if (copieCMD == NULL)
            {
                throw new runtime_error("Comanda DROP sintaxa nerecunoscuta!");
            }
            if (copieCMD->cuvant == "TABLE")
            {
                // parser table
                return cmdDROP[0]->parse(cmd);
            }
            else if (copieCMD->cuvant == "INDEX")
            {
                // parser index
                return cmdDROP[1]->parse(cmd);
            }
            else
            {
                throw new runtime_error("Comanda DROP sintaxa nerecunoscuta!");
            }
        }
        else
        {
            return 2;
        }
    }
};

class ParserComenzi
{
    int amexit = 0;
    // 1 daca a fost parsata comanda exit

    void copiere(int amexit)
    {
        this->amexit = amexit;
    }
public:
    ParserComenzi()
    {
        // preinitializat
    }

    ParserComenzi& operator=(const ParserComenzi& w)
    {
        copiere(w.amexit);
        return *this;
    }

    bool hasquit()
    {
        return (amexit == 1);
    }

    void parse(string line)
    {
        ListaCuvinte* cmd;
        cmdGENERIC* tipuricmd[9];
        //cate o clasa pentru fiecare tip de comanda

        if (line == string("exit"))
        {
            // iesire in caz de vede "exit"
            amexit = 1;
            return;
        }

        // se transforma dintr-o propozitie in ceva ce poate fi procesat
        // se despart cuvintele in mod recursiv
        // mereu vom avea un cuvant si adresa urmmatorului (alocat dinamic)
        cmd = mklista(line);

        tipuricmd[0] = new cmdCREATEx();
        tipuricmd[1] = new cmdDROPx();
        tipuricmd[2] = new cmdDISPLAYTABLE();
        tipuricmd[3] = new cmdINSERT();
        tipuricmd[4] = new cmdDELETE();
        tipuricmd[5] = new cmdSELECT();
        tipuricmd[6] = new cmdUPDATE();
        tipuricmd[7] = new cmdIMPORT();

        try
        {
            int p = 1;
            for (int i = 0; i < 8; i++)
            {
                // se trimite linia de la tastatura spre toate comenzile pentru a fi parsata
                p *= tipuricmd[i]->parse(cmd);
            }

            if (p)
            {
                cout << "\nCOMANDA NERECUNOSCUTA!\n";
            }
        }
        catch (exception* ex)
        {
            cout << "\n" << ex->what();
            delete ex;
            cout << "\n";
        }
    }
    ~ParserComenzi()
    {
        // nimic de sters
    }
};

// clasa citire fisiere
class citireFisiere
{
    ParserComenzi pc;
    string line;
public:
    // parcurge fisiere introduse ca argument in main
    void parcurgereFisiere(int nrFisiere, char** denFisiere)
    {
        for (int i = 1; i < nrFisiere; i++)
        {
            ifstream f(denFisiere[i]);
            parserFisier(f, denFisiere[i]);
        }
        cout << "\n##############################";
        cout << "\nTOATE FISIERELE AU FOST PROCESATE CU SUCCES!";
        cout << "\nSE ASTEAPTA COMENZI DE LA TASTATURA";
        cout << "\n##############################\n";
    }

    // procesare fisiere introduse ca argument in main
    void parserFisier(ifstream& f, char* nume)
    {
        if (f)
        {
            cout << "\n##############################";
            cout << "\nSE INCEPE PROCESAREA FISIERULUI: " << nume;
            cout << "\n##############################";
            // acelasi loop ca in main doar ca se opreste cand se termina fisierul, nu cand se tasteaza exit
            do
            {
                getline(f, line);
                pc.parse(line);
            } while (!f.eof());
            f.close();
            cout << "\n##############################";
            cout << "\nFISIERUL " << nume << " A FOST PROCESAT";
            cout << "\n##############################";
        }
        else
        {
            cout << "\n##############################";
            cout << "\nFISIERUL " << nume << " NU POATE FI DESCHIS!";
            cout << "\n##############################";
        }
    }
};

// functia principala
int main(int argc, char** argv)
{
    ParserComenzi pc;
    string line;
    citireFisiere cf;
    bool fisiere = false;

    // se verifica daca exista fisiere trimise ca argument
    cout << "##############################";
    cout << "\nPENTRU A IESI DIN PROGRAM SE FOLOSESTE 'exit'\n";
    cout << "SE ASTEAPTA COMENZI PENTRU TESTARE...";
    if (argc >= 2 && argc <= 6)
    {
        fisiere = true;
        cout << "\nAU FOST GASITE " << argc - 1 << " FISIERE:\n";
        for (int i = 1; i < argc; i++)
        {
            cout << argv[i] << " ";
        }
    }
    else if (argc >= 6)
    {
        cout << "\nAU FOST GASITE MAI MULT DE 5 FISIERE. ACESTEA VOR FI IGNORATE!";
    }
    cout << "\n##############################";

    // daca exista, se parcurg
    if (fisiere)
    {
        cf.parcurgereFisiere(argc, argv);
    }
    else
    {
        cout << endl;
    }

    // daca nu, se face bucla principala (care se face inclusiv dupa parcurgerea tutror fisierelor)
    do
    {
        getline(cin, line);
        pc.parse(line);
    } while (!pc.hasquit());
    return 0;
}
// cd C:\Users\Dragos\source\repos\proiectPOOindividual\x64\Debug
// start proiectPOOindividual.exe configtest.txt