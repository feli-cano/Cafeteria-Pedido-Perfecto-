// Cafeteria "Pedido Perfecto": Builder + Decorator + Observer en un solo flujo.
// Compilar: g++ -std=c++17 -Wall -o cafeteria cafeteria.cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// ===== ABSTRACCION COMPARTIDA: Bebida =====
struct Bebida {
	virtual ~Bebida() = default;
	virtual string descripcion() const = 0;
	virtual int costo() const = 0;
};

struct Cafe : Bebida { // Componente base
	string nombre; 
	int precio;
	Cafe(string n, int p) : nombre(move(n)), precio(p) {}
	string descripcion() const override { return nombre; }
	int costo() const override { return precio; }
};

// =================================================================
// 2. ESTRUCTURAL: Decorator (Envuelve una Bebida y le suma extras)
// =================================================================
struct Extra : Bebida {
protected:
	unique_ptr<Bebida> base; 
	string nombre; 
	int precio;
public:
	Extra(unique_ptr<Bebida> b, string n, int p)
		: base(move(b)), nombre(move(n)), precio(p) {}
	string descripcion() const override { return base->descripcion() + " + " + nombre; }
	int costo() const override { return base->costo() + precio; }
};

struct LecheAvena : Extra {
	LecheAvena(unique_ptr<Bebida> b) : Extra(move(b), "leche de avena", 1500) {}
};
struct ShotExtra : Extra {
	ShotExtra(unique_ptr<Bebida> b) : Extra(move(b), "shot extra", 2000) {}
};
struct Crema : Extra {
	Crema(unique_ptr<Bebida> b) : Extra(move(b), "crema", 1000) {}
};

struct Pedido {
	string cliente;
	unique_ptr<Bebida> bebida; // Bebida ya decorada entregada por el Builder
};

// =================================================================
// 1. CREACIONAL: Builder (Arma el Pedido paso a paso y apila Decorators)
// =================================================================
class PedidoBuilder {
	string cliente_; 
	unique_ptr<Bebida> bebida_;
public:
	PedidoBuilder& paraCliente(string c) { cliente_ = move(c); return *this; }
	PedidoBuilder& base(string nombre, int precio) {
		bebida_ = make_unique<Cafe>(move(nombre), precio); 
		return *this;
	}
	PedidoBuilder& conAvena() { bebida_ = make_unique<LecheAvena>(move(bebida_)); return *this; }
	PedidoBuilder& conShot()  { bebida_ = make_unique<ShotExtra>(move(bebida_));  return *this; }
	PedidoBuilder& conCrema() { bebida_ = make_unique<Crema>(move(bebida_));     return *this; }
	
	Pedido build() { 
		return Pedido{move(cliente_), move(bebida_)}; 
	}
};

// =================================================================
// 3. COMPORTAMIENTO: Observer (La barra notifica eventos a todos)
// =================================================================
struct Observador {
	virtual ~Observador() = default;
	virtual void actualizar(const string& evento, const Pedido& p) = 0;
};

class Barra { // Sujeto Observable
	vector<Observador*> observadores;
public:
	void suscribir(Observador* o) { observadores.push_back(o); }
	void notificar(const string& evento, const Pedido& p) {
		for (auto* o : observadores) o->actualizar(evento, p);
	}
};

struct Barista : Observador {
	void actualizar(const string& e, const Pedido& p) override {
		if (e == "NUEVO") cout << "[Barista] -> Preparar: " << p.bebida->descripcion() << "\n";
	}
};

struct Pantalla : Observador {
	void actualizar(const string& e, const Pedido& p) override {
		cout << "[Pantalla] -> " << p.cliente << ": pedido " << e << "\n";
	}
};

struct Caja : Observador {
	int total = 0;
	void actualizar(const string& e, const Pedido& p) override {
		if (e == "NUEVO") {
			total += p.bebida->costo();
			cout << "[Caja] -> Cobrar $" << p.bebida->costo() << " (Acumulado en caja: $" << total << ")\n";
		}
	}
};

// ===== GESTOR Y MENU INTERACTIVO =====
class CafeteriaSistema {
	vector<Pedido> historialPedidos;
	Barra barra;
	Barista barista;
	Pantalla pantalla;
	Caja caja;
	
public:
	CafeteriaSistema() {
		barra.suscribir(&barista);
		barra.suscribir(&pantalla);
		barra.suscribir(&caja);
	}
	
	void registrarPedido(Pedido p) {
		cout << "\n--- PROCESANDO NUEVO PEDIDO EN LA BARRA ---" << endl;
		barra.notificar("NUEVO", p);
		barra.notificar("LISTO", p);
		historialPedidos.push_back(move(p));
		cout << "-------------------------------------------\n" << endl;
	}
	
	void verHistorial() const {
		cout << "\n========== HISTORIAL DE PEDIDOS ==========" << endl;
		if (historialPedidos.empty()) {
			cout << "No hay pedidos registrados en la sesion actual.\n";
		} else {
			int i = 1;
			for (const auto& p : historialPedidos) {
				cout << i++ << ". Cliente: " << p.cliente 
					<< " | Bebida: " << p.bebida->descripcion() 
					<< " | Costo Total: $" << p.bebida->costo() << endl;
			}
		}
		cout << "==========================================\n" << endl;
	}
};

int main() {
	CafeteriaSistema sistema;
	int opcion = 0;
	
	do {
		cout << "=== CAFETERIA 'PEDIDO PERFECTO' ===" << endl;
		cout << "1. Registrar nuevo pedido" << endl;
		cout << "2. Ver historial de pedidos" << endl;
		cout << "3. Salir" << endl;
		cout << "Seleccione una opcion: ";
		cin >> opcion;
		
		if (cin.fail()) {
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Opcion invalida. Intente de nuevo.\n";
			continue;
		}
		
		if (opcion == 1) {
			string nombreCliente, tipoCafe;
			int precioBase;
			
			cout << "Nombre del cliente: ";
			cin >> ws;
			getline(cin, nombreCliente);
			
			cout << "Tipo de cafe base (ej. Espresso, Latte, Capuchino): ";
			getline(cin, tipoCafe);
			
			cout << "Precio base del cafe ($): ";
			cin >> precioBase;
			
			PedidoBuilder builder;
			builder.paraCliente(nombreCliente).base(tipoCafe, precioBase);
			
			char agregar;
			cout << "Agregar leche de avena ($1500)? (s/n): ";
			cin >> agregar;
			if (agregar == 's' || agregar == 'S') builder.conAvena();
			
			cout << "Agregar shot extra ($2000)? (s/n): ";
			cin >> agregar;
			if (agregar == 's' || agregar == 'S') builder.conShot();
			
			cout << "Agregar crema ($1000)? (s/n): ";
			cin >> agregar;
			if (agregar == 's' || agregar == 'S') builder.conCrema();
			
			sistema.registrarPedido(builder.build());
			
		} else if (opcion == 2) {
			sistema.verHistorial();
		} else if (opcion == 3) {
			cout << "Saliendo del sistema...\n";
		} else {
			cout << "Opcion no valida. Intente de nuevo.\n";
		}
		
	} while (opcion != 3);
	
	return 0;
}
