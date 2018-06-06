// 08-12-2015
// billy
// webo.c++
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fstream>
#include <vector>
#include <bsl/cadena>
#include <bsl/map>
#include <bsl/pila>
using namespace std;
using namespace bsl;

struct Link {
  Cadena nom;
  Cadena link;
  int carp;
  Link();
  Link(const Cadena & a, const Cadena & b, int c);
  friend ostream & operator<<(ostream & eix, const Link & l);
};

struct Carpeta {
  Cadena nom;
  int num;
  int pare;
  Carpeta();
  Carpeta(const Cadena & a, int b, int c);
  friend ostream & operator<<(ostream & eix, const Carpeta & c);
};

struct Motor {
  Cadena nom;
  Cadena url;
  Cadena url_post;
  Motor();
  Motor(const Cadena & a, const Cadena & b, const Cadena & c);
  friend ostream & operator<<(ostream & eix, const Motor & m);
};

ostream & operator<<(ostream & eix, const Link & l);
ostream & operator<<(ostream & eix, const Carpeta & c);
ostream & operator<<(ostream & eix, const Motor & m);
vector<vector<Cadena> > arguments(int num_arg, char * vec_arg[]);
void print_car(const Map<Cadena, Carpeta> & car);
void print_lin(const Map<Cadena, Carpeta> & car, const Map<Cadena, Link> & lin);
void print_lin(const Map<Cadena, Carpeta> & car, const Map<Cadena, Link> & lin, int n);
void print_mot(const Map<Cadena, Motor> & cer);
void print_aju(const char * prog);
Cadena formatar_web(Cadena ent);
Cadena color_car(Cadena ent);
Cadena color_cmd(Cadena ent);
Cadena color_lnk(Cadena ent);
Cadena color_cerc(Cadena ent);
Cadena color_cmdm(Cadena ent);

int main (int num_arg, char * vec_arg[]) {
  Map<Cadena, Motor> cerques;
  Map<Cadena, Link> enllacos;
  Map<Cadena, Carpeta> carpetes;
  ifstream fit;
  struct passwd *pw = getpwuid(getuid());
  Cadena fitxer_conf = Cadena(pw->pw_dir) + Cadena("/.webo.cfg");
  fit.open(fitxer_conf.c_str());
  while (!fit.eof()) {
    Cadena aux;
    fit >> aux;
    if (!aux.empty() and (aux[0] != '#')) {
      vector<Cadena> tmp = aux.explode(";");
      if (tmp[0] == "mc")
	cerques[tmp[1]] = Motor(tmp[2], tmp[3], tmp[4]);
      else if (tmp[0] == "cr")
	carpetes[tmp[1]] = Carpeta(tmp[2], tmp[3].cad2i(), tmp[4].cad2i());
      else if (tmp[0] == "en")
	enllacos[tmp[1]] = Link(tmp[2], tmp[3], tmp[4].cad2i());
    }
  }
  fit.close();

  if ((num_arg == 1) or (strcmp(vec_arg[1], "-h") == 0)) {
    print_aju(vec_arg[0]);
    return -1;
  }
  else if ((num_arg == 2) and (strcmp(vec_arg[1], "-vc") == 0)) {
    print_car(carpetes);
    return -2;
  }
  else if ((num_arg == 2) and (strcmp(vec_arg[1], "-vl") == 0)) {
    print_lin(carpetes, enllacos);
    return -3;
  }  
  else if ((num_arg == 3) and (strcmp(vec_arg[1], "-vl") == 0)) {
    Cadena cad = Cadena(vec_arg[2]).substr(1);
    vector<Cadena> claus = carpetes.key_vec();
    vector<Carpeta> valor = carpetes.value_vec();
    bool trobat = false;
    unsigned int i = 0;
    for (i = 0; i < claus.size() && !trobat; i++)
      if (cad == claus[i])
	trobat = true;
    if (trobat)
      print_lin(carpetes, enllacos, valor[i - 1].num);
    return -3;
  }
  else if ((num_arg == 2) and (strcmp(vec_arg[1], "-vm") == 0)) {
    print_mot(cerques);
    return -4;
  }
  else if ((num_arg > 2) and (vec_arg[1][0] == '-') and ((vec_arg[1][1] == 'n') or (vec_arg[1][1] == 'd')) and ((vec_arg[1][2] == 'm') or (vec_arg[1][2] == 'c') or (vec_arg[1][2] == 'l'))) {
    bool canvis = false;
    Cadena arg_ed = vec_arg[1];
    if (arg_ed == "-nm") {
      if ((vec_arg[2][0] == '-') and (vec_arg[2][1] == '-') and ((num_arg == 5) or (num_arg == 6))) {
	Cadena clau_mot = Cadena(vec_arg[2]).substr(2);
	Motor cerca;
	if (cerques.search(clau_mot, cerca))
	  clog << "El motor de cerca " << clau_mot << " ja existeix." << endl;
	else {
	  cerques[clau_mot] = Motor(Cadena(vec_arg[3]), Cadena(vec_arg[4]), num_arg == 6 ? Cadena(vec_arg[5]) : Cadena(""));
	  cout << "Motor " << clau_mot << " afegit correctament." << endl;
	  canvis = true;
	}
      }
      else
	clog << "S'ha d'utilitzar:" << endl << vec_arg[0] << " -nm <--motor> <nom> <url_pre> <url_post>" << endl;
    }
    else if (arg_ed == "-dm") {
      if ((vec_arg[2][0] == '-') and (vec_arg[2][1] == '-') and (num_arg == 3)) {
	Cadena clau_mot = Cadena(vec_arg[2]).substr(2);
	if (cerques.erase(clau_mot)) {
	  cout << "El motor de cerca " << clau_mot << " ha sigut esborrat." << endl;
	  canvis = true;
	}
	else
	 clog << "El motor de cerca " << clau_mot << " no existeix." << endl; 
      }
      else
	clog << "S'ha d'utilitzar:" << endl << vec_arg[0] << " -dm <--motor>" << endl;   
    }
    else if (arg_ed == "-nc") {
      if ((num_arg >= 4) and (num_arg <= 5)) {
	int num_pare = 0;
	if (num_arg == 5) {
	  Cadena clau_pare = Cadena(vec_arg[4]).substr(1);
	  Carpeta carp;
	  if (carpetes.search(clau_pare, carp))
	    num_pare = carp.num;
	  else {
	    clog << "La carpeta " << clau_pare << " no existeix." << endl;
	    num_pare = -1;
	  }
	}
	if (num_pare >= 0) {
	  Cadena clau_carp = Cadena(vec_arg[2]).substr(1);
	  if (clau_carp.size() >= 3) {
	    Carpeta carp;
	    if (carpetes.search(clau_carp, carp))
	      clog << "La carpeta " << clau_carp << " ja existeix." << endl;
	    else {
	      int maxim_carp = 0;
	      vector<Carpeta> vec_ca = carpetes.value_vec();
	      for (unsigned int i = 0; i < vec_ca.size(); i++)
		if (vec_ca[i].num > maxim_carp)
		  maxim_carp = vec_ca[i].num;
	      carpetes[clau_carp] = Carpeta(Cadena(vec_arg[3]), maxim_carp + 1, num_pare);
	      cout << "Carpeta " << clau_carp << " afegida correctament." << endl;
	      canvis = true;
	    }
	  }
	  else
	    clog << "La carpeta ha de tenir com a mínim 3 caràcters a banda del guionet (-)." << endl;
	}
      }
      else
	clog << "S'ha d'utilitzar:" << endl << vec_arg[0] << " -nc <-carp> <nom> <-carp_pare>" << endl;
    }
    else if (arg_ed == "-dc") {
      if ((num_arg == 3) and (vec_arg[2][0] == '-')) {
	Cadena clau_carp = Cadena(vec_arg[2]).substr(1);
	Carpeta carp;
	if (carpetes.search(clau_carp, carp)) {
	  bool buida = true;
	  vector<Carpeta> vec_car = carpetes.value_vec();
	  for (unsigned int i = 0; i < vec_car.size() and buida; i++)
	    if (vec_car[i].pare == carp.num)
	      buida = false;
	  if (buida) {
	    vector<Link> vec_lin = enllacos.value_vec();
	    for (unsigned int i = 0; i < vec_lin.size() and buida; i++)
	      if (vec_lin[i].carp == carp.num)
		buida = false;
	  }
	  if (buida) {
	    if (carpetes.erase(clau_carp)) {
	      cout << "La carpeta " << clau_carp << " ha sigut esborrada." << endl;
	      canvis = true;
	    }
	    else
	      clog << "La carpeta " << clau_carp << " no s'ha pogut esborrar." << endl;
	  }
	  else
	    clog << "La carpeta " << clau_carp << " no s'ha pogut esborrar perquè no esta buida." << endl;
	}
	else
	  clog << "La carpeta " << clau_carp << " no existeix." << endl;
      }
      else
	clog << "S'ha d'utilitzar:" << endl << vec_arg[0] << " -dc <-carp>" << endl;
    }
    else if (arg_ed == "-nl") {
      if ((num_arg == 6) and (vec_arg[2][0] == '-') and (vec_arg[5][0] == '-')) {
	Cadena clau_carp = Cadena(vec_arg[5]).substr(1);
	Carpeta carp;
	if (carpetes.search(clau_carp, carp)) {
	  int pare = carp.num;
	  Cadena clau_link = Cadena(vec_arg[2]).substr(1);
	  Link enllac;
	  if (enllacos.search(clau_link, enllac))
	    clog << "El enllaç " << clau_link << " ja existeix." << endl;
	  else {
	    enllacos[clau_link] = Link(Cadena(vec_arg[3]), Cadena(vec_arg[4]), pare);
	    cout << "Enllaç " << clau_link << " (" << vec_arg[4] << ") afegit correctament." << endl;
	    canvis = true;
	  }
	}
	else
	  clog << "La carpeta " << clau_carp << " no existeix." << endl;
      }
      else
	clog << "S'ha d'utilitzar:" << endl << vec_arg[0] << " -nl <-link> <nom> <url> <-carp>" << endl;
    }
    else if (arg_ed == "-dl") {
      if ((num_arg == 3) and (vec_arg[2][0] == '-')) {
	Cadena clau_link = Cadena(vec_arg[2]).substr(1);
	if (enllacos.erase(clau_link)) {
	  cout << "L'enllaç " << clau_link << " ha sigut esborrat." << endl;
	  canvis = true;
	}
	else
	  clog << "L'enllaç " << clau_link << " no existeix." << endl;
      }
      else
	clog << "S'ha d'utilitzar:" << endl << vec_arg[0] << " -dl <-link>" << endl;
    }
    if (canvis) {
      ofstream fit_eix;
      fit_eix.open(fitxer_conf.c_str());
      vector<Cadena> kcar = carpetes.key_vec();
      vector<Cadena> klin = enllacos.key_vec();
      vector<Cadena> kcer = cerques.key_vec();
      vector<Carpeta> vcar = carpetes.value_vec();
      vector<Link> vlin = enllacos.value_vec();
      vector<Motor> vcer = cerques.value_vec();
      for (unsigned int i = 0; i < kcar.size(); i++)
	fit_eix << "cr;" << kcar[i] << ";" << vcar[i] << endl;
      for (unsigned int i = 0; i < klin.size(); i++)
	fit_eix << "en;" << klin[i] << ";" << vlin[i] << endl;
      for (unsigned int i = 0; i < kcer.size(); i++)
	fit_eix << "mc;" << kcer[i] << ";" << vcer[i] << endl;
      fit_eix.close();
    }
  }
  else {
    Cadena ret = "/usr/bin/iceweasel";
    Cadena ret_ini = ret;
    vector<vector<Cadena> > argu = arguments(num_arg, vec_arg);
    for (unsigned int i = 1; i < argu.size(); i++) {
      if ((argu[i][0][0] == '-') and (argu[i][0][1] == '-')) {
	Motor motor_cerca;
	if (cerques.search(argu[i][0].substr(2), motor_cerca))
	  for (unsigned int j = 1; j < argu[i].size(); j++)
	    ret = ret + " -new-tab " + "\"" + motor_cerca.url + formatar_web(argu[i][j]) + motor_cerca.url_post + "\"";
      }
      else if (argu[i][0][0] == '-') {
	Link enllac;
	if (enllacos.search(argu[i][0].substr(1), enllac))
	  ret = ret + " -new-tab " + "\"" + enllac.link + "\"";
	else {
	  Carpeta grup_enllac;
	  if (carpetes.search(argu[i][0].substr(1), grup_enllac)) {
	    vector<Link> links = enllacos.value_vec();
	    for (unsigned int j = 0; j < links.size(); j++)
	      if (links[j].carp == grup_enllac.num)
		ret = ret + " -new-tab " + "\"" + links[j].link + "\"";
	  }
	}
      }
    }
    if (ret != ret_ini) {
      ret = ret + " 2> /dev/null";
      if (fork() == 0)
	system(ret.c_str());
    }
  }
  return 0;
}

Link::Link() {
  nom = "";
  link = "";
  carp = -1;
}

Link::Link(const Cadena & a, const Cadena & b, int c) {
  nom = a;
  link = b;
  carp = c;
}

Carpeta::Carpeta() {
  nom = "";
  num = -1;
  pare = -1;
}

Carpeta::Carpeta(const Cadena & a, int b, int c) {
  nom = a;
  num = b;
  pare = c;
}

Motor::Motor() {
  nom = "";
  url = "";
  url_post = "";
}

Motor::Motor(const Cadena & a, const Cadena & b, const Cadena & c) {
  nom = a;
  url = b;
  url_post = c;
}

ostream & operator<<(ostream & eix, const Link & l) {
  eix << l.nom << ";" << l.link << ";" << l.carp;
  return eix;
}

ostream & operator<<(ostream & eix, const Carpeta & c) {
  eix << c.nom << ";" << c.num << ";" << c.pare;
  return eix;
}

ostream & operator<<(ostream & eix, const Motor & m) {
  eix << m.nom << ";" << m.url << ";" << m.url_post;
  return eix;
}

vector<vector<Cadena> > arguments(int num_arg, char * vec_arg[]) {
  vector<vector<Cadena> > ret;
  vector<Cadena> elem;
  for (int i = 0; i < num_arg; i++) {
    if (((strlen(vec_arg[i]) > 2) and (vec_arg[i][0] == '-') and (vec_arg[i][1] == '-') and (vec_arg[i][2] != '-')) or ((strlen(vec_arg[i]) > 1) and (vec_arg[i][0] == '-') and (vec_arg[i][1] != '-'))) {
      ret.push_back(elem);
      elem.clear();
    }
    elem.push_back(Cadena(vec_arg[i]));
  }
  ret.push_back(elem);
  return ret;
}

void print_car(const Map<Cadena, Carpeta> & car) {
  vector<Cadena> kcar = car.key_vec();
  vector<Carpeta> vcar = car.value_vec();
  int pare = 0;
  Pila<int> anteriors;
  anteriors.push(pare);
  while (!vcar.empty()) {
    unsigned int i = 0;
    while (i < vcar.size() && vcar[i].pare != pare)
      i++;
    if (i < vcar.size()) {
      for (int s = 1; s < anteriors.size(); s++)
	clog << "  |";
      clog << "-";
      clog << color_car(vcar[i].nom) << " " << color_cmd(kcar[i]) << endl;
      anteriors.push(pare);
      pare = vcar[i].num;
      vcar.erase(vcar.begin() + i);
      kcar.erase(kcar.begin() + i);
    }
    else {
      pare = anteriors.top();
      anteriors.pop();
    }
  }
}

void print_lin(const Map<Cadena, Carpeta> & car, const Map<Cadena, Link> & lin) {
  vector<Cadena> kcar = car.key_vec();
  vector<Carpeta> vcar = car.value_vec();
  vector<Cadena> klin = lin.key_vec();
  vector<Link> vlin = lin.value_vec();
  int pare = 0;
  Pila<int> anteriors;
  anteriors.push(pare);
  while (!vcar.empty()) {
    unsigned int i = 0;
    while (i < vcar.size() && vcar[i].pare != pare)
      i++;
    if (i < vcar.size()) {
      for (int s = 1; s < anteriors.size(); s++)
	clog << "  |";
      clog << "-";
      clog << color_car(vcar[i].nom) << " " << color_cmd(kcar[i]) << endl;
      anteriors.push(pare);
      pare = vcar[i].num;
      for (unsigned int j = 0; j < klin.size(); j++)
	if (vlin[j].carp == pare) {
	  for (int s = 1; s < anteriors.size(); s++)
	    clog << "  |";
	  clog << "-";	  
	  clog << color_lnk(vlin[j].nom) << " " << color_cmd(klin[j]) << endl;
	}
      vcar.erase(vcar.begin() + i);
      kcar.erase(kcar.begin() + i);
    }
    else {
      pare = anteriors.top();
      anteriors.pop();
    }
  }
}

void print_lin(const Map<Cadena, Carpeta> & car, const Map<Cadena, Link> & lin, int n) {
  vector<Carpeta> vcar = car.value_vec();
  vector<Cadena> klin = lin.key_vec();
  vector<Link> vlin = lin.value_vec();
  bool trobat = false;
  for (unsigned int i = 0; i < vcar.size() and !trobat; i++)
    if (vcar[i].num == n) {
      clog << color_car(vcar[i].nom) << ":" << endl;
      trobat = true;
    }
  for (unsigned int j = 0; j < klin.size(); j++)
    if (vlin[j].carp == n)
      clog << color_lnk(vlin[j].nom) << " " << color_cmd(klin[j]) << endl;
}

void print_mot(const Map<Cadena, Motor> & cer) {
  vector<Cadena> kcer = cer.key_vec();
  vector<Motor> vcer = cer.value_vec();
  for (unsigned int i = 0; i < kcer.size(); i++)
    clog << color_cerc(vcer[i].nom) << " " << color_cmdm(kcer[i]) << endl;
}

void print_aju(const char * prog) {
  clog << prog << " v1.0" << endl;
  clog << endl;
  clog << "\033[1;31mOpcions d'edició:\033[0m" << endl;
  clog << "  -nm <--motor> <nom> <url_pre> <url_post>\tAfegir motor de cerca" << endl;
  clog << "  -dm <--motor>                           \tEsborrar motor de cerca" << endl;
  clog << "  -nc <-carp> <nom> <-carp_pare>          \tAfegir carpeta" << endl;
  clog << "  -dc <-carp>                             \tEsborrar carpeta" << endl;
  clog << "  -nl <-link> <nom> <url> <-carp>         \tAfegir enllaç" << endl;
  clog << "  -dl <-link>                             \tEsborrar enllaç" << endl;
  clog << endl;
  clog << "\033[1;31mOpcions de visualització:\033[0m" << endl;
  clog << "  -h                                      \tMostrar aquesta ajuda" << endl;
  clog << "  -vc                                     \tVeure carpetes" << endl;
  clog << "  -vl                                     \tVeure carpetes i links" << endl;
  clog << "  -vl <-carp>                             \tVeure links de la carpeta \"-carp\"" << endl;
  clog << "  -vm                                     \tVeure motors de cerca" << endl;
  clog << endl;
  clog << "\033[1;31mExemple:\033[0m" << endl;
  clog << "  " << prog << " --cerca \"prova de cerca\"\t\tCercar \"prova de cerca\" en el motor --cerca" << endl;
  clog << "  " << prog << " -lnk1 -lnk2 -lnk3         \t\tObrir els enllaços -lnk1 -lnk2 -lnk3" << endl;
  clog << "  " << prog << " -lnk1 --cerca asdf -lnk2  \t\tObrir els enllaços -lnk1 -lnk2 i cercar \"asdf\"" << endl;
}

Cadena formatar_web(Cadena ent) {
  Cadena eix = ent;
  eix = eix.str_replace(" ", "+");
  eix = eix.str_replace("/", "%2F");
  eix = eix.str_replace(":", "%3A");
  eix = eix.str_replace("?", "%3F");
  eix = eix.str_replace(",", "%2C");
  return eix;
}

Cadena color_car(Cadena ent) {
  return "\033[1;34m" + ent + "\033[0m";
}

Cadena color_cmd(Cadena ent) {
  return "\033[0;32m-" + ent + "\033[0m";
}

Cadena color_lnk(Cadena ent) {
  return "\033[1;37m" + ent + "\033[0m";
}

Cadena color_cerc(Cadena ent) {
  return "\033[1;35m" + ent + "\033[0m";
}

Cadena color_cmdm(Cadena ent) {
  return "\033[0;32m--" + ent + "\033[0m";
}
