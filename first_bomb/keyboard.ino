int rows[]={9,8,7,6};   //указываем пины строк
int cols[]={5,4,3,2};   //и столбцов

void keyboard_init() {

        Serial.begin(9600);
        Serial.println("Starting...");
        for (int i=0; i<sizeof(rows); i++){
          pinMode(rows[i], INPUT);
        }
        for (int i=0; i<sizeof(cols); i++){
          pinMode(cols[i], OUTPUT);
        }
}

//массив с соответствиями кодов нажатых кнопок
char simbols[][2]={
        {0,'1'},
        {1,'2'},
        {2,'3'},
        {3,'A'},
        {4,'4'},
        {5,'5'},
        {6,'6'},
        {7,'B'},
        {8,'7'},
        {9,'8'},
        {10,'9'},
        {11,'C'},
        {12,'*'},
        {13,'0'},
        {14,'#'},
        {15,'D'}
};

char keyboard_get_key() {

        for(int i=0; i<4; i++) { //цикл для перехода по всем строкам
                digitalWrite(rows[i], HIGH); //подаем на текущую строку высокий уровень
                for(int j=0; j<4; j++) { //цикл для переходов по всем столбцам
                        if(digitalRead(cols[j])==HIGH) { //если уровень высокий, то кнопка нажата
                                return _FindKey(4*i+j);
                        }
                }
                digitalWrite(rows[i], LOW); //выключаем высокий уровень для пройденной строки
        }
}
//функция возвращающая код нажатой кнопки
char _FindKey(unsigned int keyCode){
        for(int i=0; i<16; i++) {
                if(simbols[i][0]==keyCode) {
                        return simbols[i][1];
                }
        }
        return '-'; //если код на найден возвращаем ‘-‘
}
