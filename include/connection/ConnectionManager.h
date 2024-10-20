/*
 * File: ConnectionManager.h
 * Author: Ashutosh Vishwakarma
 * Company: Robogen
 * Date: 20-10-2024
 * Description: This file contains ConnectionManager interface class. All the connection managers will inherit fdrom it.
 * Copyright (c) 2024 Robogen
 *
 * This file is part of the Robogen project.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Proprietary and confidential.
 */


#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#include <string>
class ConnectionManager{
public:
  virtual bool establish(std::string *to)=0;
  virtual void send(std::string msg)=0;
  virtual void recieve(std::string *msg)=0;
  virtual ~ConnectionManager() {}
};
#endif // CONNECTION_MANAGER_H
