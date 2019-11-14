//Basead em http://stdcxx.apache.org/doc/stdlibug/2-2.html#225
 
#include <queue>
#include <iostream>
#include <cstdlib>
#include <math.h>
 
unsigned int a = 1103515245;
unsigned int c = 123456;
unsigned int m =2147483648;
unsigned int seed = 0;
double t;

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
 
void simulation::run () {
 
  while (! eventQueue.empty() ) { //enquanto exisitr eventos na fila de eventos
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

float U(){
	seed = (a*seed+c)% m;
	return ((float)seed/m);
}

class simuladorFilas : public simulation {
public:
	simuladorFilas() {
		s0=s1=s2=false;
		counter0=counter1=counter2=0;	
		lambc0 = 0.2;
		lambc1 = 0.3;
		lambc2 = 0.4;
		lambs0 = 0.3;
		lambs1 = 0.5;
		lambs2 = 0.6;
	} //aqui vai criar os estados de cada fila: ocupado, livre, etc
	std::queue<event *> aux0,aux1,aux2; //filas auxiliares
	bool s0,s1,s2;
	int counter0,counter1,counter2,total;
	double lambc0,lambc1,lambc2,lambs0, lambs1, lambs2;

} oSimulador;

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
	chegada0 *evc0 = new chegada0(t=t+exp(oSimulador.lambc0));
	oSimulador.scheduleEvent(evc0);	
	if(oSimulador.s0 == false) oSimulador.s0 = true;
	else oSimulador.aux0.push(evc0);
	if(oSimulador.aux2.empty()) oSimulador.s2 = false;
	else oSimulador.aux2.pop();
}

void chegada2::processEvent() {	
	saida2 *evs2 = new saida2(t=t+exp(oSimulador.lambs2));
	oSimulador.scheduleEvent(evs2);	
	if(oSimulador.s2 == false){
		oSimulador.s2 = true;
	}else{
		oSimulador.aux2.push(evs2);
	}	
	oSimulador.counter2++;
	oSimulador.total++;
}
 
void saida1::processEvent() { 
	chegada0 *evc0 = new chegada0(t=t+exp(oSimulador.lambc0));
	oSimulador.scheduleEvent(evc0);	
	if(oSimulador.s0 == false) oSimulador.s0 = true;
	else oSimulador.aux0.push(evc0);
	if(oSimulador.aux1.empty()) oSimulador.s1 = false;
	else oSimulador.aux1.pop();
}

void chegada1::processEvent() {
	saida1 *evs1 = new saida1(t=t+exp(oSimulador.lambs1));
	oSimulador.scheduleEvent(evs1);	
	if(oSimulador.s1 == false){
		oSimulador.s1 = true;
	}else{
		oSimulador.aux1.push(evs1);
	}
	oSimulador.counter1++;
	oSimulador.total++;
}
 
void saida0::processEvent() {
	double ii;
	ii = fRand();
	if(ii > 0.5){
		std::cout << "oi1:  " << ii<< " : "<<time<<'\n';
		chegada1 *evc1 = new chegada1(t=t+exp(oSimulador.lambc1));
		oSimulador.scheduleEvent(evc1);
		if(oSimulador.s1 == false) oSimulador.s1 = true;
		else oSimulador.aux1.push(evc1);
	}else if(ii < 0.3){
		std::cout << "oi2:  " <<ii<< " : "<<time<<'\n';
		chegada2 *evc2 = new chegada2(t=t+exp(oSimulador.lambc2));
		oSimulador.scheduleEvent(evc2);
		if(oSimulador.s2 == false) oSimulador.s2 = true;
		else oSimulador.aux2.push(evc2);
	}else {
		chegada0 *evc0 = new chegada0(t=t+exp(oSimulador.lambc0));
		oSimulador.scheduleEvent(evc0);
		std::cout << "oi3:  " << ii<< " : "<<time<<'\n';
	}
	if(oSimulador.aux0.empty()) oSimulador.s0 = false;
	else oSimulador.aux0.pop();
}
 
void chegada0::processEvent() { 
	if(oSimulador.total < 1000){
		saida0 *evs0 = new saida0(t=t+exp(oSimulador.lambs0));
		oSimulador.scheduleEvent(evs0);	
		if(oSimulador.s0 == false){
			std::cout << "oi0:  " << " : "<<time<< '\n';
			oSimulador.s0 = true;
		}else{
			std::cout << "oi00:  " << " : "<<time<< '\n';
			oSimulador.aux0.push(evs0);
		}
		oSimulador.counter0++;
		oSimulador.total++;
	}
}

//////////////////////// main ////////////////////////////////////////
int main() { 
	oSimulador.scheduleEvent(new chegada0 (t=exp(oSimulador.lambc0)));//mudar o tempo para ser exponencial
	oSimulador.run (); // inicia simulador	
	std::cout << "eventos0 " << oSimulador.counter0  << '\n';
	std::cout << "eventos1 " << oSimulador.counter1  << '\n';
	std::cout << "eventos2 " << oSimulador.counter2  << '\n';
	std::cout << "total " << oSimulador.total  << '\n';
	return 0;
}
