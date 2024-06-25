/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatimi <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 15:14:27 by afatimi           #+#    #+#             */
/*   Updated: 2024/06/25 15:53:19 by ylyoussf         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include<Replies.hpp>

void Replies::RPL_WELCOME(Client &client, Server &server)
{
	FD fd = client.getFdObject();

	static_cast<void>(server);

	string reply = "001 ";
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
