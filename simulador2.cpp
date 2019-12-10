//Basead em http://stdcxx.apache.org/doc/stdlibug/2-2.html#225
 
#include <queue>
#include <iostream>
#include <cstdlib>
#include <math.h>
 
unsigned int a = 1103515245;
unsigned int c = 12345;
unsigned int m = 2147483648;
unsigned int seed = 5560;

//==================================================================
// classes evento e simulação - base para o desenvolvimento da simulação
//==================================================================
class event {
public:
  event (double t) : time (t)  { }
  virtual void processEvent () = 0; 
  const double time;
  double ti;
}; 
 
class simulation {
public:
  simulation() : simtime(0), eventQueue()  {}
  void run();
  void  scheduleEvent(event * newEvent) {eventQueue.push(newEvent);}
  double simtime;
protected:
  class eventComparator {
    public:
    bool operator() (const event * left, const event * right) const {
      return left->time > right->time;
    }
  };
  std::priority_queue<event*,
                      std::vector<event *, std::allocator<event*> >,
                      eventComparator> eventQueue;  //fila de eventos 
};

class simuladorFilas : public simulation {
public:
	simuladorFilas() {
		s0=s1=s2=false;
		counter0=counter1=counter2=descartado=0;	
		lambc0 = 0.1;
		lambc1 = 0.1;
		lambc2 = 0.1;
		lambs0 = 0.7;
		lambs1 = 0.7;
		lambs2 = 0.7;
		m_f0=m_f1=m_f2=m_tf1=m_tf2=0;
	}
	std::queue<event *> aux0,aux1,aux2; //filas auxiliares
	bool s0,s1,s2; //estado dos servidores de cada fila
	int counter0, counter1, counter2, descartado; //contador de eventos em cada fila
	double lambc0, lambc1, lambc2, lambs0, lambs1, lambs2, m_f0, m_f1, m_f2, m_tf1, m_tf2;
} oSimulador;

void simulation::run () { 
  while (oSimulador.counter0 <= 10000) { //enquanto exisitr eventos na fila de eventos
    event * nextEvent = eventQueue.top(); //captura evento no topo da fila
    eventQueue.pop ();  //retira evento da fila
	simtime = nextEvent->time;  //ajusta tempo de simulação
    nextEvent->processEvent();  //processa evento	
    delete nextEvent;  //remove evento
  }
}
 
//===================================================================
//Customização dos eventos e do simulador
//===================================================================	

double fRand() {
	//trocar pelo nosso gerador de numeros uniformes
    seed = (a*seed+c)% m;
	return ((double)seed/m);
}

double exp(double lamb){
	return (log(fRand())/(-lamb));
}

class chegada0 : public event {
public:
	chegada0(double t) : event(t)  { }
	virtual void processEvent ();
};

class saida0 : public event {
public:
	saida0(double t) : event(t)  { }
	virtual void processEvent ();
};

class chegada1 : public event {
public:
	chegada1(double t) : event(t)  { }
	virtual void processEvent ();
};

class saida1 : public event {
public:
	saida1(double t) : event(t)  { }
	virtual void processEvent ();
};

class chegada2 : public event {
public:
	chegada2(double t) : event(t)  { }
	virtual void processEvent ();
};

class saida2 : public event {
public:
	saida2(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void saida2::processEvent() {
	std::cout << "es2: " << oSimulador.simtime  << '\n';
	if(oSimulador.aux2.empty()) 
		oSimulador.s2 = false;
	else{
		saida2 *evs2 = new saida2(oSimulador.simtime+exp(oSimulador.lambs2));
		oSimulador.m_f2 = oSimulador.m_f2+(evs2->time-oSimulador.aux2.front()->time);		
		oSimulador.m_tf2 = oSimulador.m_tf2+(evs2->time-oSimulador.aux2.front()->ti);
		oSimulador.scheduleEvent(evs2);
		oSimulador.aux2.pop();
	} 
}

void chegada2::processEvent() {
	std::cout << "ec2: " << oSimulador.simtime  << '\n';
	oSimulador.counter2++;
	if(oSimulador.s2 == false){
		saida2 *evs2 = new saida2(oSimulador.simtime+exp(oSimulador.lambs2));
		oSimulador.m_f2 = oSimulador.m_f2+(evs2->time-this->time);
		oSimulador.m_tf2 = oSimulador.m_tf2+(evs2->time-this->ti);
		oSimulador.scheduleEvent(evs2);
		oSimulador.s2 = true;
	}else{
		chegada2 *evc2 = new chegada2(this->time);
		evc2->ti = this->ti;
		oSimulador.aux2.push(evc2);
	}	
}
 
void saida1::processEvent() {
	std::cout << "es1: " << oSimulador.simtime  << '\n';
	if (oSimulador.aux1.empty()){
		oSimulador.s1 = false;
	}else{
		saida1 *evs1 = new saida1(oSimulador.simtime+exp(oSimulador.lambs1));
		oSimulador.m_f1 = oSimulador.m_f1+(evs1->time-oSimulador.aux1.front()->time);		
		oSimulador.m_tf1 = oSimulador.m_tf1+(evs1->time-oSimulador.aux1.front()->ti);
		oSimulador.scheduleEvent(evs1);
		oSimulador.aux1.pop();
	}
}

void chegada1::processEvent() {
	std::cout << "ec1: " << oSimulador.simtime  << '\n';
	oSimulador.counter1++;	
	if(oSimulador.s1 == false){
		saida1 *evs1 = new saida1(oSimulador.simtime+exp(oSimulador.lambs1));
		oSimulador.m_f1 = oSimulador.m_f1+(evs1->time-this->time);
		oSimulador.m_tf1 = oSimulador.m_tf1+(evs1->time-this->ti);
		oSimulador.scheduleEvent(evs1);
		oSimulador.s1 = true;
	}else{
		chegada1 *evc1 = new chegada1(this->time);
		evc1->ti = this->ti;
		oSimulador.aux1.push(evc1);
	}
}
 
void saida0::processEvent() {
	std::cout << "es0: " << oSimulador.simtime  << '\n';
	double ii = fRand();
	if(ii > 0.5){
		//chegada1 *evc1 = new chegada1(oSimulador.simtime+exp(oSimulador.lambc1));
		chegada1 *evc1 = new chegada1(oSimulador.simtime);
		evc1->ti=this->ti;	
		oSimulador.scheduleEvent(evc1);	
	}else if(ii < 0.3){
		//chegada2 *evc2 = new chegada2(oSimulador.simtime+exp(oSimulador.lambc2));
		chegada2 *evc2 = new chegada2(oSimulador.simtime);
		evc2->ti=this->ti;
		oSimulador.scheduleEvent(evc2);
	}else{
		oSimulador.descartado++;
	}
	
	if(oSimulador.aux0.empty()) 
		oSimulador.s0 = false;
	else{
		saida0 *evs0 = new saida0(oSimulador.simtime+exp(oSimulador.lambs0));
		evs0->ti = oSimulador.aux0.front()->time;
		oSimulador.scheduleEvent(evs0);
		oSimulador.aux0.pop();
	}

	oSimulador.scheduleEvent(new chegada0 (oSimulador.simtime+exp(oSimulador.lambc0)));
}

void chegada0::processEvent() {	
	std::cout << "ec0: " << oSimulador.simtime  << '\n';	
	oSimulador.counter0++;				
	if(oSimulador.s0 == false){
		saida0 *evs0 = new saida0(oSimulador.simtime+exp(oSimulador.lambs0));
		oSimulador.m_f0 = oSimulador.m_f0+(evs0->time-this->time);
		evs0->ti = this->time;
		oSimulador.scheduleEvent(evs0);
		oSimulador.s0 = true;
	}else oSimulador.aux0.push(new chegada0(this->time));
}
 
////////////////////////////// main ////////////////////////////////////////
int main() {
	oSimulador.scheduleEvent(new chegada0 (oSimulador.simtime+exp(oSimulador.lambc0)));
	oSimulador.run (); //inicia simulador
	std::cout << "Resultaos: ------------------------------------"<< '\n';
	std::cout << "eventos q0: " << oSimulador.counter0  << '\n';
	std::cout << "media tempo q0: " << oSimulador.m_f0/oSimulador.counter0  << '\n';
	std::cout << "eventos q1: " << oSimulador.counter1  << '\n';
	std::cout << "media tempo q1: " << oSimulador.m_f1/oSimulador.counter1  << '\n';
	std::cout << "media tempo sistema q1: " << oSimulador.m_tf1/oSimulador.counter1  << '\n';
	std::cout << "eventos q2: " << oSimulador.counter2  << '\n';
	std::cout << "media tempo q2: " << oSimulador.m_f2/oSimulador.counter2  << '\n';
	std::cout << "media tempo sistema q2: " << oSimulador.m_tf2/oSimulador.counter2  << '\n';
	std::cout << "eventos descartados: " << oSimulador.descartado  << '\n';
	return 0;
}