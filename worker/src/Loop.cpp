#define MS_CLASS "Loop"

#include "Loop.h"
#include "DepLibUV.h"
#include "MediaSoupError.h"
#include "Logger.h"
#include <string>
#include <csignal>  // sigfillset, pthread_sigmask()
#include <cstdlib>  // std::genenv()
#include <cerrno>

/* Instance methods. */

Loop::Loop()
{
	MS_TRACE();

	int channelFd = std::stoi(std::getenv("MEDIASOUP_CHANNEL_FD"));

	// Set the signals handler.
	this->signalsHandler = new SignalsHandler(this);

	// Add signals to handle.
	this->signalsHandler->AddSignal(SIGINT, "INT");
	this->signalsHandler->AddSignal(SIGTERM, "TERM");

	// Set the Channel socket.
	this->channel = new Channel::UnixStreamSocket(this, channelFd);

	MS_DEBUG("starting libuv loop");
	DepLibUV::RunLoop();
	MS_DEBUG("libuv loop ended");
}

Loop::~Loop()
{
	MS_TRACE();
}

void Loop::Close()
{
	MS_TRACE();

	int err;
	sigset_t signal_mask;

	if (this->closed)
	{
		MS_ERROR("already closed");
		return;
	}
	this->closed = true;

	// First block all the signals to not be interrupted while closing.
	sigfillset(&signal_mask);
	err = pthread_sigmask(SIG_BLOCK, &signal_mask, nullptr);
	if (err)
		MS_ERROR("pthread_sigmask() failed: %s", std::strerror(errno));

	// Close the SignalsHandler.
	this->signalsHandler->Close();

	// Close the Channel socket.
	this->channel->Close();

	// Close all the Rooms.
	for (auto& kv : this->rooms)
	{
		RTC::Room* room = kv.second;

		room->Close();
	}
}

void Loop::onSignal(SignalsHandler* signalsHandler, int signum)
{
	MS_TRACE();

	switch (signum)
	{
		case SIGINT:
			MS_DEBUG("signal INT received, exiting");
			Close();
			break;

		case SIGTERM:
			MS_DEBUG("signal TERM received, exiting");
			Close();
			break;

		default:
			MS_WARN("received a signal (with signum %d) for which there is no handling code", signum);
	}
}

void Loop::onSignalsHandlerClosed(SignalsHandler* signalsHandler)
{
	MS_TRACE();
}

void Loop::onChannelRequest(Channel::UnixStreamSocket* channel, Channel::Request* request)
{
	MS_TRACE();

	switch (request->methodId)
	{
		case Channel::Request::MethodId::createRoom:
		{
			MS_DEBUG("'createRoom' method");
			break;
		}

		case Channel::Request::MethodId::createPeer:
		{
			MS_DEBUG("'createPeer' method");
			break;
		}

		default:
		{
			MS_ABORT("unexpected methodId");
		}
	}





	// TODO: TMP jeje

	if (request->id < 25000)
	{
		request->Accept();
	}
	else if (request->id < 50000)
	{
		Json::Value data;

		data["jojojo"] = "🐮🐷🐣😡";
		request->Accept(data);
	}
	else if (request->id < 75000)
	{
		std::string reason = "this is a std::string reason";

		request->Reject(400, reason);
	}
	else
	{
		request->Reject(500, "this is a char* reason");
	}
}

void Loop::onChannelUnixStreamSocketRemotelyClosed(Channel::UnixStreamSocket* socket)
{
	MS_TRACE();

	// When mediasoup Node process ends it sends a SIGTERM to us so we close this
	// pipe. If the pipe is remotely closed it means that mediasoup Node process
	// abrutly died so we must die.

	MS_ERROR("Channel remotely closed, killing myself");

	Close();
}