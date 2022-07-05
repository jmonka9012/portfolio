#include <iostream>     // std::cout                                               Projekt zaliczeniowy - Sztczna Inteligencja (wykład): Nauka AI gry w oczko          
#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <chrono> 
#include <deque>
#include <list>
#include <numeric>
#include <functional>
#include <iomanip>
#include <conio.h>

using namespace std;

#define RED(x) "\033[1;31m" << x << "\033[0m"
#define BLUE(x) "\033[1;34m" << x << "\033[0m"
#define GREEN(x) "\033[1;32m" << x << "\033[0m"
#define BROWN(x) "\033[1;33m" << x << "\033[0m"
#define ROUND(x, r) fixed << setprecision(r) << x
#define Pc(p, q, Kp, Kq) "P[" << p << "]" << "[" << q << "]" << "[" << Kp << "]" << "[" << Kq << "]"
#define Nc(p, q, Kp, Kq) "N[" << p << "]" << "[" << q << "]" << "[" << Kp << "]" << "[" << Kq << "]"

typedef unsigned number;

enum card {
    two = 2, three, four,
    five, six, seven,
    eight, nine, ten,
    jack = 2, queen = 3,
    king = 4, ace = 11
};

void print(const list<number> args) {
    for (const number& item : args)
        cout << item << "\t";
    cout << endl;
}

void Shuffle_Deck(deque<card>& deck) {
    number seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(deck.begin(), deck.end(), default_random_engine(seed));
}

deque<card> New_deck() {
    return deque<card> {
        two, two, two, two,
            three, three, three, three,
            four, four, four, four,
            five, five, five, five,
            six, six, six, six,
            seven, seven, seven, seven,
            eight, eight, eight, eight,
            nine, nine, nine, nine,
            ten, ten, ten, ten,
            jack, jack, jack, jack,
            queen, queen, queen, queen,
            king, king, king, king,
            ace, ace, ace, ace};
}

card Draw(deque<card>& deck) {
    card c = deck.front();
    deck.pop_front();
    return c;
}

number Suma_oczek(deque<card>& reka) {
    number s = 0;
    for (card& x : reka) s += x;
    return s;
}

bool Czy_przegral(deque<card>& reka) {
    if (Suma_oczek(reka) == 22 && reka.size() == 2)
        return false;
    else if (Suma_oczek(reka) > 21)
        return true;
    else
        return false;
}

bool Koniec_partii(deque<card>& gracz1, deque<card>& gracz2, bool gracz1_stop, bool gracz2_stop) {
    if (gracz1_stop == 1 && gracz2_stop == 1)
        return true;
    else if (Czy_przegral(gracz1) || Czy_przegral(gracz2))
        return true;
    else
        return false;
}

void Fill(float* tensor, const list<number> shape, number value = 0) {
    number size = accumulate(shape.begin(), shape.end(), 1, multiplies<number>());
    fill((number*)tensor, (number*)tensor + size, value);
}

void print_reka(deque<card> reka) {
    for (card& x : reka) {
        cout << x << "\t";
    }
}

float P[23][23][10][10]; 
float N[23][23][10][10];

deque<card> gracz_AI;
deque<card> gracz;
char znak;
number tempP{ 0 };

int main() {

    bool stop_AI = false;
    bool stop_pl = false;
    bool diagnostyka = false;
    number n{ 0 };
    cout << BROWN("Program uczy AI gry w oczko") << endl << endl;

    cout << BROWN("Legenda: ") << endl;
    cout << "p - ilosc oczek gracza p" << endl;
    cout << "q - ilosc oczek gracza q" << endl;
    cout << "Kp - liczba kart na rece gracza p" << endl;
    cout << "Kq - liczba kart na rece gracza q" << endl;
    cout << "P[p][q][Kp][Kq] - prawdopodobienstwo na porazke po dobraniu karty dla danego stanu gry aktualizowane na podstawie wynikow partii symulacyjnych (uczacych) oraz dalszych rozgrywek" << endl;
    cout << "N[p][q][Kp][Kq] - liczba wystapien danego stanu gry" << endl << endl;


    cout << "Wprowadz liczbe symulacji, na podstawie ktorych algorytm ma sie uczyc n -> ";
    cin >> n;

    cout << "Czy wyswietlac na biezaco aktualizacje P (prawdopodobienstwa porazki po dobraniu karty) i N (liczby powtorzen dla danego stanu gry)?  T - tak\n";
    znak = _getch();

    diagnostyka = (znak == 'T' || znak == 't') ? 1 : 0;

    cout << "\nSymuluje...\n";

    Fill((float*)P, { 23, 23, 10, 10 }, 0);
    Fill((float*)N, { 23, 23, 10, 10 }, 1);

    while (n--) {
        
        stop_AI = false;
        gracz_AI.clear();
        gracz.clear();
        deque<card> deck = New_deck();
        Shuffle_Deck(deck);

        do {
            number
                sum_ai = Suma_oczek(gracz_AI),
                sum_pl = Suma_oczek(gracz),
                hand_ai = gracz_AI.size(),
                hand_pl = gracz.size();

            // wskaźniki na wartości przed dobraniem karty
            float* probability = &P[sum_pl][sum_ai][hand_pl][hand_ai];
            float* counts = &N[sum_pl][sum_ai][hand_pl][hand_ai];
               

            gracz.push_front(Draw(deck));
            *counts += 1;
            *probability += (Czy_przegral(gracz) - *probability) / *counts;   //aktualizacja średniej

            if (diagnostyka) {
                cout << RED(ROUND(*probability, 2)) << "\t";
                cout << BLUE(ROUND(*counts, 0)) << "\t";
                print({ sum_ai, sum_pl, hand_ai, hand_pl });
            }

            if (Czy_przegral(gracz)) break;
           
            
            sum_ai = Suma_oczek(gracz_AI);
            sum_pl = Suma_oczek(gracz);
            hand_ai = gracz_AI.size();
            hand_pl = gracz.size();

            // wskaźniki na wartości przed dobraniem karty
            probability = &P[sum_ai][sum_pl][hand_ai][hand_pl];
            counts = &N[sum_ai][sum_pl][hand_ai][hand_pl];

            if (!stop_AI) {
                if (*probability <= 0.5) {
                    gracz_AI.push_front(Draw(deck));
                    *counts += 1;
                    *probability += (Czy_przegral(gracz_AI) - *probability) / *counts;
                }
                else
                    stop_AI = true;

                // diagnostyka
                if (diagnostyka) {
                    cout << RED(ROUND(*probability, 2)) << "\t";
                    cout << BLUE(ROUND(*counts, 0)) << "\t";
                    print({ sum_ai, sum_pl, hand_ai, hand_pl });
                }
            }
            

        } while (!Koniec_partii(gracz_AI, gracz, stop_AI, 0));
    }


    cout << "\nSymulacja zakonczona pomyslnie!" << endl << endl;

    do {
        deque<card> deck = New_deck();
        number p, q, Kp, Kq{ 0 };
        

        cout << BROWN("\nMenu wyboru: ") << endl;
        cout << "1. Wypisz wybrane P[p][q][Kp][Kq] oraz N[p][q][Kp][Kq]" << endl;
        cout << "2. Rozegraj partie gry w oczko z AI" << endl;
        cout << "3. Zakoncz dzialanie programu" << endl;
        
        do {
            znak = _getch();
        } while (znak != '1' && znak != '2' && znak != '3');

        switch (znak) {
        case '1':
            cout << "\np -> "; cin >> p;
            cout << "q -> "; cin >> q;
            cout << "Kp -> "; cin >> Kp;
            cout << "Kq -> "; cin >> Kq;

            cout << endl << Pc(p, q, Kp, Kq) << " = " << RED(ROUND(P[p][q][Kp][Kq],2));
            cout << endl << Nc(p, q, Kp, Kq) << " = " << BLUE(ROUND(N[p][q][Kp][Kq], 0)) << endl;
            break;
        case '2':
            stop_pl = false;
            stop_AI = false;
            gracz_AI.clear();
            gracz.clear();
            Shuffle_Deck(deck);
            do {
                number
                    sum_ai = Suma_oczek(gracz_AI),
                    sum_pl = Suma_oczek(gracz),
                    hand_ai = gracz_AI.size(),
                    hand_pl = gracz.size();

                // wskaźniki na wartości przed dobraniem karty
                float* probability = &P[sum_pl][sum_ai][hand_pl][hand_ai];
                float* counts = &N[sum_pl][sum_ai][hand_pl][hand_ai];

                if (!stop_pl) {
                    cout << BROWN("\n\nTwoja reka to:\t"); print_reka(gracz);
                    cout << BROWN("\nTwoja suma oczek to: ");cout << Suma_oczek(gracz);
                    cout << "\nPrzeciwnik AI ma " << Suma_oczek(gracz_AI) << " oczek w " << gracz_AI.size() << " kartach" << endl << endl;
                    cout << "\nD - Dobierz karte | Z - Zakoncz ture";
                    znak = NULL;
                    while (znak != 'D' && znak != 'd' && znak != 'Z' && znak != 'z') znak = _getch();
                    if (znak == 'D' || znak == 'd') {
                        gracz.push_front(Draw(deck));
                    }
                    else stop_pl = true;
                }

                *counts += 1;
                *probability += (Czy_przegral(gracz) - *probability) / *counts;

                // diagnostyka
                cout << endl << endl << RED(ROUND(*probability, 2)) << "\t";
                cout << BLUE(ROUND(*counts, 0)) << "\t";
                print({ sum_pl, sum_ai, hand_pl, hand_ai });

                if (Czy_przegral(gracz)) {
                    cout << RED("\nPrzegrales!") << endl;
                    cout << "\nTwoja suma oczek: " << Suma_oczek(gracz) << endl;
                    cout << "Suma oczek przeciwnika AI: " << Suma_oczek(gracz_AI) << endl;
                    break;
                }

                sum_ai = Suma_oczek(gracz_AI);
                sum_pl = Suma_oczek(gracz);
                hand_ai = gracz_AI.size();
                hand_pl = gracz.size();

                // wskaźniki na wartości przed dobraniem karty
                probability = &P[sum_ai][sum_pl][hand_ai][hand_pl];
                counts = &N[sum_ai][sum_pl][hand_ai][hand_pl];

                if (stop_AI == false) {
                    if (*probability <= 0.5) {
                        gracz_AI.push_front(Draw(deck));
                        cout << "\nPrzeciwnik AI dobral karte";
                        *counts += 1;
                        *probability += (Czy_przegral(gracz_AI) - *probability) / *counts;
                    }
                    else
                    {
                        cout << "\nPrzeciwnik AI spasowal";
                        stop_AI = true;
                    }
                }

                cout << endl << endl << RED(ROUND(*probability, 2)) << "\t";
                cout << BLUE(ROUND(*counts, 0)) << "\t";
                print({ sum_ai, sum_pl, hand_ai, hand_pl });

                if (Czy_przegral(gracz_AI)) {
                    cout << GREEN("\nWygrales!") << endl;
                    cout << "\nTwoja suma oczek: " << Suma_oczek(gracz) << endl;
                    cout << "Suma oczek przeciwnika AI: " << Suma_oczek(gracz_AI) << endl;
                    break;
                }

                if (stop_pl == 1 && stop_AI == 1)
                {
                    if (Suma_oczek(gracz) > Suma_oczek(gracz_AI))
                        cout << GREEN("\nWygrales!") << endl;
                    else if (Suma_oczek(gracz) == Suma_oczek(gracz_AI))
                        cout << BROWN("\nRemis") << endl;
                    else 
                        cout << RED("\nPrzegrales!") << endl;
                }
                
            } while (!Koniec_partii(gracz_AI, gracz, stop_AI, stop_pl));
            break;
        case '3':
            break;
        }
    
    } while (znak != '3');

    return 0;
}