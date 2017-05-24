const int kierunekPinLewyA=7;
const int kierunekPinLewyB=8;
const int kierunekPinPrawyA=11;
const int kierunekPinPrawyB=12;

const int pwmPinLewy=10;
const int pwmPinPrawy=9;

const int czujnikPinLewy=0;
const int czujnikPinProstoA=1;
const int czujnikPinProstoB=2;
const int czujnikPinPrawy=3;

const int dioda=13;

const int prog_prosto=50;
const int prog_bok=100;

int czujnikLewy;
int czujnikProstoA;
int czujnikProstoB;
int czujnikPrawy;

//odleglosci z czujnikow
int os;
int front;

//pozycja poczatkowa
int x=1;
int y=1;

//orientacja: 0-góra,1-prawo,2-dół,3-lewo;
int o=0;

//wylaczenie pinow sterusjacych silnikiem
void zerujKierunek(){
    digitalWrite(kierunekPinLewyA,LOW);
    digitalWrite(kierunekPinLewyB,LOW);
    digitalWrite(kierunekPinPrawyA,LOW);
    digitalWrite(kierunekPinPrawyB,LOW);
}

//odbliczenie roznicy odleglosci pomiedzy sciana lewa i prawa
void odleglosci_boki(){
    int wynik=0;
    int wynik_temp=0;
    int czujnikLewyTab=0;
    int czujnikPrawyTab=0;
    
    //pobranie 10 pomiarow
    for(int w=1;w<=10;w++)
    {
        delay(40);
        czujnikLewyTab=analogRead(czujnikPinLewy);
        czujnikPrawyTab=analogRead(czujnikPinPrawy);
        
        wynik_temp=czujnikPrawyTab-czujnikLewyTab;
        
        wynik=wynik+wynik_temp;
    }
    //obliczenie sredniej
    os=wynik/10;
}

//obliczenie odleglosci do sciany frontowej
void odleglosci_prosto(){

    int czujnikProstoASuma=0;
    //pobranie 25 pomiarow z czujnika A
    for(int w=1;w<=25;w++)
    {
        int czujnikProstoATab=analogRead(czujnikPinProstoA);
        delay(1);
        czujnikProstoASuma=czujnikProstoASuma+czujnikProstoATab;
    }
    
    int czujnikProstoBSuma=0;
    //pobranie 25 pomiarow z czujnika B
    for(int w=1;w<=25;w++)
    {
        int czujnikProstoBTab=analogRead(czujnikPinProstoB);
        delay(1);
        czujnikProstoBSuma=czujnikProstoBSuma+czujnikProstoBTab;
}

    //obliczenie sredniej
    czujnikProstoA=czujnikProstoASuma/25;
    czujnikProstoB=czujnikProstoBSuma/25;
    front=czujnikProstoA-czujnikProstoB;
  
}

//jazda o jedno pole do przedu-funkcja
void jazdaProsto(){
    float j1;
    float j2;
    
    //przypisanie odpowiedniego kierunku
    digitalWrite(kierunekPinLewyA,HIGH);
    digitalWrite(kierunekPinLewyB,LOW);
    digitalWrite(kierunekPinPrawyA,LOW);
    digitalWrite(kierunekPinPrawyB,HIGH);
    
    //wywolanie funkcji ktora oblicza roznice odleglosci pomiedzy scianami
    odleglosci_boki();
    
    //wartosc PWM prawego silnika
    j2=200;
    
    //obliczenie wartosci PWM lewego silnika
    if(os<-20) j2=0.05*os+186;
    else if(os>20) j2=0.05*os+184;
    else j2=185;
    
    analogWrite(pwmPinLewy,j1);
    analogWrite(pwmPinPrawy,j2);
    delay(300);  
    
    //wylaczenie silnikow
    analogWrite(pwmPinLewy,0);
    analogWrite(pwmPinPrawy,0);
    zerujKierunek();
    
    delay(1000);
}

//obrot w prawo - funkcja
void obrotPrawo(){
  
    //wlaczenie silnikow
    analogWrite(pwmPinLewy,50);
    analogWrite(pwmPinPrawy,50);
    
    digitalWrite(kierunekPinLewyA,LOW);
    digitalWrite(kierunekPinLewyB,HIGH);
    digitalWrite(kierunekPinPrawyA,HIGH);
    digitalWrite(kierunekPinPrawyB,LOW);
    
    //wywolanie funkcji ktora odczyta odleglosc o sciany frontowej
    odleglosci_prosto();
  
    //regulacja dokladnosci obrotu
    int dokrec_sie=0;
    if (front>=0) dokrec_sie=front/2.5;
    else dokrec_sie=front/3;
    
    delay(213-dokrec_sie);
    
    //wylaczenie silnikow
    analogWrite(pwmPinLewy,0);
    analogWrite(pwmPinPrawy,0);
    zerujKierunek();
    
    delay(1000);
}

void setup() {
  
    pinMode (kierunekPinLewyA,OUTPUT);
    pinMode (kierunekPinLewyB,OUTPUT);
    pinMode (kierunekPinPrawyA,OUTPUT);
    pinMode (kierunekPinPrawyB,OUTPUT);
    
    pinMode (dioda,OUTPUT);
  
    pinMode (pwmPinLewy,OUTPUT);
    pinMode (pwmPinPrawy,OUTPUT);
  
    Serial.begin(9600); 
}

void loop() {
    //opoznienie poczatkowe
    delay(5000);
    
    //najpierw jedz jedno pole do przodu
    jazdaProsto();
    
    //aktualizacja pozycji
    y=2;
  
    //petla dopki pole docelowe nie jest osiagniete
    while(x!=8 || y!=8){
  
        delay(1000);
        
        //odczyt wartosci z czujnikow odleglosci
        czujnikLewy=analogRead(czujnikPinLewy);
        czujnikProstoA=analogRead(czujnikPinProstoA);
        czujnikProstoB=analogRead(czujnikPinProstoB);
        czujnikPrawy=analogRead(czujnikPinPrawy);
      
        //czy mozna obrócić się w prawo?
      
        if(czujnikPrawy<prog_bok){
            
            //obroc w prawo i jedz do przodu
            obrotPrawo();
            jazdaProsto();
            
            //aktualizacja orientacji
            o++;
            if(o==4) o=0;
          
            //aktualizacja pozycji
            if(o==0) y++;
            if(o==1) x++;
            if(o==2) y--;
            if(o==3) x--;
      }
      
      //jeśli nie prawo, to czy można jechać prosto?
      
      else if(czujnikProstoA<prog_prosto){
            //jedz prosto
            jazdaProsto();
            
            //ustalenie pozycji
            if(o==0) y++;
            if(o==1) x++;
            if(o==2) y--;
            if(o==3) x--;
      }
      
      //jeśli nie prosto, to czy mozna obrócić się w lewo?
      
      else if(czujnikLewy<prog_bok){
            
            //skrec w lewo
            digitalWrite(kierunekPinLewyA,HIGH);
            digitalWrite(kierunekPinLewyB,LOW);
            digitalWrite(kierunekPinPrawyA,LOW);
            digitalWrite(kierunekPinPrawyB,HIGH);
            delay(200);
            zerujKierunek();
            delay(1000);
            
            //jedz prosto
            jazdaProsto();
        
            //aktualizacja orientacji
            o--;
            if(o<0) o=3;
            
            //aktualizacja pozycji
            if(o==0) y++;
            if(o==1) x++;
            if(o==2) y--;
            if(o==3) x--;
      }
      
      //jesli slepy zaulek to obroc się i jedz do przodu
      
      else{
            //obroc sie o kat 180st
            digitalWrite(kierunekPinLewyA,HIGH);
            digitalWrite(kierunekPinLewyB,LOW);
            digitalWrite(kierunekPinPrawyA,LOW);
            digitalWrite(kierunekPinPrawyB,HIGH);
            delay(360);
            zerujKierunek();
            delay(1000);
            
            //jedz prosto
            jazdaProsto();

            //aktualizacja orientacji
            if(o==0) o=2;
            if(o==1) o=3;
            if(o==2) o=0;
            if(o==3) o=1;
            
            //aktualizacja pozycji
            if(o==0) y++;
            if(o==1) x++;
            if(o==2) y--;
            if(o==3) x--;
      }
  }
    
    //wylaczenie silnikow
    analogWrite(pwmPinLewy,0);
    analogWrite(pwmPinPrawy,0);
    
    //jesli dojechano co celu
    while(1){
        //sygnalizacja dioda
        digitalWrite(dioda,HIGH);
        delay(250);
        digitalWrite(dioda,LOW);
        delay(250);
    }
}
