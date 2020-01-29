#ifndef __COMM_CONFIG_H__
#define __COMM_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../base/thread.h"
#include "../base/socket.h"
#include "../base/sistema.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"

#include "./soap-config.h"
#include "./users-config.h"
#include "./comm-gen-config.h"
#include "./comm-ser-config.h"
#include "./comm-har-config.h"
#include "./comm-res-config.h"
#include "./comm-uv5-config.h"

/** Estructura de CONFIGURACION. Se parte de REDAN */
class CommConfig : public jData
{
public:
	CommConfig() {
		/** Modo para las configuraciones por defecto */
		tipo = LocalConfig::p_cfg->get(strRuntime, strRuntimeItemModoGlobal, "0")=="1" ? 1 : 0;
		idConf=string("defecto_") + (tipo==0 ? string("redan") : string("ulises"));
		fechaHora = Tools::Ahora_Servidor();
	}
	CommConfig(string jstring) {
											// Configuracion desde REDAN
		JDeserialize(jstring);
		tipo = 0;							
	}
	CommConfig(ifstream &f) {               // Configuracion desde fichero JSON
		string data,linea;
		while (std::getline(f, linea))
			data += linea;
		JDeserialize(data);
	}
	CommConfig(soap_config &sConfig);		// Configuracion desde servidor ULISES. (el tipo se pone a 1)
	~CommConfig() {
		clear_array(users);
		clear_array(recursos);
	}
//public:
//	bool operator == (CommConfig &otra) {
//		return (idConf == otra.idConf && fechaHora == otra.fechaHora) ? true : false;
//	}
//	bool operator < (CommConfig &otra) {
//		struct tm time_l,time_o;
//		time_t loctime,othtime;
//
//		Tools::DateString2time(fechaHora, time_l);
//		Tools::DateString2time(otra.fechaHora, time_o);
//
//		loctime = mktime(&time_l);
//		othtime = mktime(&time_o);
//
//		return loctime < othtime;	
//	}
public:
	bool IpColateral(string &ip) {
		if (general.dualidad != 0) 
		{
			string ippropia;
			if (sistema::GetIpAddress(ippropia))
			{
				return general.IpColateral(ippropia, ip);
			}	
			PLOG_ERROR("CommConfig::IpColateral: ERROR. NO IP PROPIA !!!");
		}
		return false;
	}
	bool test() {
		bool bret = general.test() && servicios.test() && hardware.test();
		for (vector<CommResConfig>::iterator res = recursos.begin(); res != recursos.end(); res++)
			bret = bret && res->test();
		return bret;
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "tipo", tipo);
		write_key(writer, "idConf", idConf);
		write_key(writer, "fechaHora", fechaHora);
		write_key(writer, "general", general);
		write_key(writer, "hardware", hardware);
		write_key(writer, "recursos", recursos);
		write_key(writer, "users", users);
		write_key(writer, "servicios", servicios);
		write_key(writer, "ulises", ulises);
	}
	virtual void jread(Value &base) {
		read_key(base, "tipo", tipo);
		read_key(base, "idConf", idConf);
		read_key(base, "fechaHora", fechaHora);
		read_key(base, "users", users);
		read_key(base, "general", general);
		read_key(base, "servicios", servicios);
		read_key(base, "hardware", hardware);
		read_key(base, "recursos", recursos);
		read_key(base, "ulises", ulises);
	}

public:
	int tipo;
	string idConf;
	string fechaHora;
	CommGenConfig general;
	CommSerConfig servicios;
	CommHarConfig hardware;
#if __POR_REFERENCIA__
	vector<UserData> users;
	vector<CommResConfig> recursos;
#else
	vector<UserData *> users;
	vector<CommResConfig *> recursos;
#endif
	CommUv5Config ulises;

};

/** Para los Polling de Test */
class RedanTestComm : public jData
{
public:
	RedanTestComm(string jstr) {
		JDeserialize(jstr);
		tipo = 0;
	}
public:
	bool isEqual(CommConfig &otra) {
		return (idConf == otra.idConf && fechaHora == otra.fechaHora) ? true : false;
	}
	bool isSameTime(CommConfig &otra) {
		return (fechaHora == otra.fechaHora) ? true : false;
	}
	bool isNewer(CommConfig &otra) {
		struct tm time_l,time_o;
		time_t loctime,othtime;

		Tools::DateString2time(fechaHora, time_l);
		Tools::DateString2time(otra.fechaHora, time_o);

		loctime = mktime(&time_l);
		othtime = mktime(&time_o);

		return loctime > othtime;	
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "idConf", idConf);
		write_key(writer, "fechaHora", fechaHora);
	}
	virtual void jread(Value &base) {
		read_key(base, "idConf", idConf);
		read_key(base, "fechaHora", fechaHora);
	}
public:
	int tipo;
	string idConf;
	string fechaHora;
};

#endif
