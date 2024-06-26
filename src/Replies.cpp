/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylyoussf <ylyoussf@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:14:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/26 11:48:19 by ylyoussf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<Replies.hpp>

void Replies::RPL_WELCOME(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string reply = ":";
	reply += server.getServerName();
	reply += " 001 ";
	reply += Utility::getClientName(client, server);
	reply += " :Welcome to ";
	reply += Server::serverName;
	reply += " Network, ";
	reply += client.getNick();
	reply += "\r\n";
	fd << reply;
}


void Replies::RPL_CUSTOM_CLIENT_JOINED(string channel, Client &joiner, Client &client)
{
	FD fd = client.getFdObject();

	string reply = ":";
	// string reply = "069 ";
	// reply += Utility::getClientName(client, server);
	// reply += " :";
	reply += joiner.getNick();
	reply += "!";
	reply += joiner.getUsername();
	reply += "@";
	reply += joiner.getIp();
	reply += " ";
	reply += "JOIN";
	reply += " ";
	reply += channel;
	reply += "\r\n";

	// cerr << "Sending <" << reply << ">" << endl;

	fd << reply;
}

void Replies::RPL_YOURHOST(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	string reply = ":";
	reply += server.getServerName();
	reply += " 002 ";
	reply += Utility::getClientName(client, server);
	reply += " :Your host is ";
	reply += Server::serverName;
	reply += ", running version 6.9";
	reply += "\r\n";
	fd << reply;
}