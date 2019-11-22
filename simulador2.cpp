//Basead em http://stdcxx.apache.org/doc/stdlibug/2-2.html#225
 
#include <queue>
#include <iostream>
#include <cstdlib>
#include <math.h>
 
unsigned int a = 1103515245;
unsigned int c = 12345;
unsigned int m =2147483648;
unsigned int seed = 5560;

//==================================================================
// classes evento e simulação - base para o desenvolvimento da simulação
//==================================================================

class event {
public:
  // Construct sets time of event.
  event (double t) : time (t)  { }
  
  // Execute event by invoking this method.
  virtual void processEvent () = 0; 
  const double time;
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
		lambc1 = 0.2;
		lambc2 = 0.3;
		lambs0 = 0.7;
		lambs1 = 0.8;
		lambs2 = 0.9;
	} //aqui vai criar os estados de cada fila: ocupado, livre, etc
	std::queue<event *> aux0,aux1,aux2; //filas auxiliares
	bool s0,s1,s2;
	int counter0,counter1,counter2,descartado;
	double lambc0,lambc1,lambc2,lambs0, lambs1, lambs2;

} oSimulador;

void simulation::run () {
 
  while (oSimulador.counter0 <= 1000) { //enquanto exisitr eventos na fila de eventos
    event * nextEvent = eventQueue.top(); //captura evento no topo da fila
    eventQueue.pop ();  //retira evento da fila
    simtime = nextEvent->time;  //ajusta tempo de simulação
	//std::cout << simtime<<'\n';
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
	if(oSimulador.aux2.empty()) oSimulador.s2 = false;
	else oSimulador.aux2.pop();
}

void chegada2::processEvent() {
	std::cout << "ec2: " << oSimulador.simtime  << '\n';
	saida2 *evs2 = new saida2(oSimulador.simtime+exp(oSimulador.lambs2));
	oSimulador.scheduleEvent(evs2);	
	if(oSimulador.s2 == false) oSimulador.s2 = true;
	else oSimulador.aux2.push(evs2);		
	oSimulador.counter2++;
}
 
void saida1::processEvent() { 
	std::cout << "es1: " << oSimulador.simtime  << '\n';
	if(oSimulador.aux1.empty()) oSimulador.s1 = false;
	else oSimulador.aux1.pop();
}

void chegada1::processEvent() {
	std::cout << "ec1: " << oSimulador.simtime  << '\n';
	saida1 *evs1 = new saida1(oSimulador.simtime+exp(oSimulador.lambs1));
	oSimulador.scheduleEvent(evs1);
	if(oSimulador.s1 == false) oSimulador.s1 = true;	
	else oSimulador.aux1.push(evs1);
	oSimulador.counter1++;
}
 
void saida0::processEvent() {
	std::cout << "es0: " << oSimulador.simtime  << '\n';
	double ii;
	ii = fRand();
	if(ii > 0.5){
		chegada1 *evc1 = new chegada1(oSimulador.simtime+exp(oSimulador.lambc1));
		oSimulador.scheduleEvent(evc1);
	}else if(ii < 0.3){
		chegada2 *evc2 = new chegada2(oSimulador.simtime+exp(oSimulador.lambc2));
		oSimulador.scheduleEvent(evc2);
	}else oSimulador.descartado++;
	chegada0 *evc0 = new chegada0(oSimulador.simtime+exp(oSimulador.lambc0));
	oSimulador.scheduleEvent(evc0);	
	if(oSimulador.aux0.empty()) oSimulador.s0 = false;
	else oSimulador.aux0.pop();
}
 
void chegada0::processEvent() {
	    std::cout << "ec0: " << oSimulador.simtime  << '\n';
		saida0 *evs0 = new saida0(oSimulador.simtime+exp(oSimulador.lambs0));
		oSimulador.scheduleEvent(evs0);	
		if(oSimulador.s0 == false) oSimulador.s0 = true;
		else oSimulador.aux0.push(evs0);		
		oSimulador.counter0++;
}

//////////////////////// main ////////////////////////////////////////
int main() { 
	oSimulador.simtime = oSimulador.simtime+exp(oSimulador.lambc0);
	oSimulador.scheduleEvent(new chegada0 (oSimulador.simtime));//mudar o tempo para ser exponencial
	oSimulador.run (); // inicia simulador	
	std::cout << "eventos0 " << oSimulador.counter0  << '\n';
	std::cout << "eventos1 " << oSimulador.counter1  << '\n';
	std::cout << "eventos2 " << oSimulador.counter2  << '\n';
	std::cout << "eventos descartados " << oSimulador.descartado  << '\n';
	return 0;
}
