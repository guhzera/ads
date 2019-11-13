//Basead em http://stdcxx.apache.org/doc/stdlibug/2-2.html#225
 
#include <queue>
#include <iostream>
#include <cstdlib>
 
unsigned int a = 1103515245;
unsigned int c = 12345;
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

float U(){
	seed = (a*seed+c)% m;
	return ((float)seed/m);
}

class simuladorFilas : public simulation {
public:
	simuladorFilas() {} //aqui vai criar os estados de cada fila: ocupado, livre, etc
	std::queue<event *> aux0,aux1,aux2; //filas auxiliares
	bool s0,s1,s2=false;
	int counter0,counter1,counter2,total;

} oSimulador;

class chegada0 : public event {
public:
	chegada0(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void chegada0::processEvent() { 
	saida0 *evs0 = new saida0(t=t+fRand());
	oSimulador.scheduleEvent(evs0);	
	if(oSimulador.s0 == false){
		oSimulador.s0 = true;
	}else{
		oSimulador.aux0.push(evs0);
	}
}

class saida1 : public event {
public:
	saida1(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void saida1::processEvent() { 
	chegada0 *evc0 = new chegada0(t=t+fRand());
	oSimulador.scheduleEvent(evc0);	
	//if(oSimulador.s)
}

class saida2 : public event {
public:
	saida2(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void saida2::processEvent() { 
	std::cout << "saida2: " << time <<'\n';
}

class chegada2 : public event {
public:
	chegada2(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void chegada2::processEvent() {	
	std::cout << "chegada2: " << time <<'\n';	
}

class chegada1 : public event {
public:
	chegada1(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void chegada1::processEvent() {
	saida1 *evs1 = new saida1(t=t+fRand());
	oSimulador.scheduleEvent(evs1);	
	if(oSimulador.s1 == false){
		oSimulador.s1 = true;
	}else{
		oSimulador.aux1.push(evs1);
	}
}

class saida0 : public event {
public:
	saida0(double t) : event(t)  { }
	virtual void processEvent ();
};
 
void saida0::processEvent() {
	if(fRand() > 0.5){
		chegada1 *evc1 = new chegada1(t=t+fRand());
		oSimulador.scheduleEvent(evc1);
		if(oSimulador.s1 == false) oSimulador.s1 = true;
		else oSimulador.aux1.push(evc1);
	}else if(fRand() < 0.3){
		chegada2 *evc2 = new chegada2(t=t+fRand());
		oSimulador.scheduleEvent(evc2);
		if(oSimulador.s2 == false) oSimulador.s2 = true;
		else oSimulador.aux2.push(evc2);
	}
	if(oSimulador.aux0.empty()) oSimulador.s0 = false;
	else oSimulador.aux0.pop();
}

//////////////////////// main ////////////////////////////////////////
int main() { 
	
	oSimulador.scheduleEvent(new chegada0 (t=fRand()));//mudar o tempo para ser exponencial
	oSimulador.total++;
	std::cout << "saida2: " << oSimulador.total <<'\n';
	oSimulador.run (); // inicia simulador	
	return 0;
}

