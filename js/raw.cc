#ifndef RAW_CC
#define RAW_CC

#include <stdio.h>
#include <string.h>
#include "raw.h"
#include <iostream>
#include <net/if.h>
#include <netpacket/packet.h>
using namespace std;
#define ETHER_TYPE      0x88DC
#define DEFAULT_IF      "wlp1s0"
#define MY_DEST_MAC0	0xFF
#define MY_DEST_MAC1	0xFF
#define MY_DEST_MAC2	0xFF
#define MY_DEST_MAC3	0xFF
#define MY_DEST_MAC4	0xFF
#define MY_DEST_MAC5	0xFF

#ifdef _WIN32
static char errbuf[1024];
#endif
const char* raw_strerror (int code) {
#ifdef _WIN32
	if (FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, 0, code, 0, errbuf,
			1024, NULL)) {
		return errbuf;
	} else {
		strcpy (errbuf, "Unknown error");
		return errbuf;
	}
#else
	return strerror (code);
#endif
}

static uint16_t checksum (uint16_t start_with, unsigned char *buffer,
		size_t length) {
	unsigned i;
	uint32_t sum = start_with > 0 ? ~start_with & 0xffff : 0;

	for (i = 0; i < (length & ~1U); i += 2) {
		sum += (uint16_t) ntohs (*((uint16_t *) (buffer + i)));
		if (sum > 0xffff)
			sum -= 0xffff;
	}
	if (i < length) {
		sum += buffer [i] << 8;
		if (sum > 0xffff)
			sum -= 0xffff;
	}
	
	return ~sum & 0xffff;
}

namespace raw {

static Nan::Persistent<FunctionTemplate> SocketWrap_constructor;

void InitAll (Local<Object> exports) {
	ExportConstants (exports);
	ExportFunctions (exports);

	SocketWrap::Init (exports);
}

NODE_MODULE(raw, InitAll)

NAN_METHOD(CreateChecksum) {
	Nan::HandleScope scope;
	
	if (info.Length () < 2) {
		Nan::ThrowError("At least one argument is required");
		return;
	}

	if (! info[0]->IsUint32 ()) {
		Nan::ThrowTypeError("Start with argument must be an unsigned integer");
		return;
	}
	
	uint32_t start_with = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();

	if (start_with > 65535) {
		Nan::ThrowRangeError("Start with argument cannot be larger than 65535");
		return;
	}

	if (! node::Buffer::HasInstance (info[1])) {
		Nan::ThrowTypeError("Buffer argument must be a node Buffer object");
		return;
	}
	
	Local<Object> buffer = Nan::To<Object>(info[1]).ToLocalChecked();
	char *data = node::Buffer::Data (buffer);
	size_t length = node::Buffer::Length (buffer);
	unsigned int offset = 0;
	
	if (info.Length () > 2) {
		if (! info[2]->IsUint32 ()) {
			Nan::ThrowTypeError("Offset argument must be an unsigned integer");
			return;
		}
		offset = Nan::To<Uint32>(info[2]).ToLocalChecked()->Value();
		if (offset >= length) {
			Nan::ThrowRangeError("Offset argument must be smaller than length of the buffer");
			return;
		}
	}
	
	if (info.Length () > 3) {
		if (! info[3]->IsUint32 ()) {
			Nan::ThrowTypeError("Length argument must be an unsigned integer");
			return;
		}
		unsigned int new_length = Nan::To<Uint32>(info[3]).ToLocalChecked()->Value();
		if (new_length > length) {
			Nan::ThrowRangeError("Length argument must be smaller than length of the buffer");
			return;
		}
		length = new_length;
	}
	
	uint16_t sum = checksum ((uint16_t) start_with,
			(unsigned char *) data + offset, length);

	Local<Integer> number = Nan::New<Uint32>(sum);
	
	info.GetReturnValue().Set(number);
}

NAN_METHOD(Htonl) {
	Nan::HandleScope scope;

	if (info.Length () < 1) {
		Nan::ThrowError("One arguments is required");
		return;
	}

	if (! info[0]->IsUint32 ()) {
		Nan::ThrowTypeError("Number must be a 32 unsigned integer");
		return;
	}

	unsigned int number = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	Local<Uint32> converted = Nan::New<Uint32>((unsigned int) htonl (number));

	info.GetReturnValue().Set(converted);
}

NAN_METHOD(Htons) {
	Nan::HandleScope scope;
	
	if (info.Length () < 1) {
		Nan::ThrowError("One arguments is required");
		return;
	}

	if (! info[0]->IsUint32 ()) {
		Nan::ThrowTypeError("Number must be a 16 unsigned integer");
		return;
	}
	
	unsigned int number = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	
	if (number > 65535) {
		Nan::ThrowRangeError("Number cannot be larger than 65535");
		return;
	}
	
	Local<Uint32> converted = Nan::New<Uint32>(htons (number));

	info.GetReturnValue().Set(converted);
}

NAN_METHOD(Ntohl) {
	Nan::HandleScope scope;
	
	if (info.Length () < 1) {
		Nan::ThrowError("One arguments is required");
		return;
	}

	if (! info[0]->IsUint32 ()) {
		Nan::ThrowTypeError("Number must be a 32 unsigned integer");
		return;
	}

	unsigned int number = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	Local<Uint32> converted = Nan::New<Uint32>((unsigned int) ntohl (number));

	info.GetReturnValue().Set(converted);
}

NAN_METHOD(Ntohs) {
	Nan::HandleScope scope;
	
	if (info.Length () < 1) {
		Nan::ThrowError("One arguments is required");
		return;
	}

	if (! info[0]->IsUint32 ()) {
		Nan::ThrowTypeError("Number must be a 16 unsigned integer");
		return;
	}
	
	unsigned int number = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	
	if (number > 65535) {
		Nan::ThrowRangeError("Number cannot be larger than 65535");
		return;
	}
	
	Local<Uint32> converted = Nan::New<Uint32>(htons (number));

	info.GetReturnValue().Set(converted);
}

void ExportConstants (Local<Object> target) {
	Local<Object> socket_level = Nan::New<Object>();
	Local<Object> socket_option = Nan::New<Object>();

	Nan::Set(target, Nan::New("SocketLevel").ToLocalChecked(), socket_level);
	Nan::Set(target, Nan::New("SocketOption").ToLocalChecked(), socket_option);

	Nan::Set(socket_level, Nan::New("SOL_SOCKET").ToLocalChecked(), Nan::New<Number>(SOL_SOCKET));
	Nan::Set(socket_level, Nan::New("IPPROTO_IP").ToLocalChecked(), Nan::New<Number>(IPPROTO_IP + 0));
	Nan::Set(socket_level, Nan::New("IPPROTO_IPV6").ToLocalChecked(), Nan::New<Number>(IPPROTO_IPV6 + 0));

	Nan::Set(socket_option, Nan::New("SO_BROADCAST").ToLocalChecked(), Nan::New<Number>(SO_BROADCAST));
	Nan::Set(socket_option, Nan::New("SO_RCVBUF").ToLocalChecked(), Nan::New<Number>(SO_RCVBUF));
	Nan::Set(socket_option, Nan::New("SO_RCVTIMEO").ToLocalChecked(), Nan::New<Number>(SO_RCVTIMEO));
	Nan::Set(socket_option, Nan::New("SO_SNDBUF").ToLocalChecked(), Nan::New<Number>(SO_SNDBUF));
	Nan::Set(socket_option, Nan::New("SO_SNDTIMEO").ToLocalChecked(), Nan::New<Number>(SO_SNDTIMEO));

#ifdef __linux__
	Nan::Set(socket_option, Nan::New("SO_BINDTODEVICE").ToLocalChecked(), Nan::New<Number>(SO_BINDTODEVICE));
#endif

	Nan::Set(socket_option, Nan::New("IP_HDRINCL").ToLocalChecked(), Nan::New<Number>(IP_HDRINCL));
	Nan::Set(socket_option, Nan::New("IP_OPTIONS").ToLocalChecked(), Nan::New<Number>(IP_OPTIONS));
	Nan::Set(socket_option, Nan::New("IP_TOS").ToLocalChecked(), Nan::New<Number>(IP_TOS));
	Nan::Set(socket_option, Nan::New("IP_TTL").ToLocalChecked(), Nan::New<Number>(IP_TTL));

#ifdef _WIN32
	Nan::Set(socket_option, Nan::New("IPV6_HDRINCL").ToLocalChecked(), Nan::New<Number>(IPV6_HDRINCL));
#endif
	Nan::Set(socket_option, Nan::New("IPV6_TTL").ToLocalChecked(), Nan::New<Number>(IPV6_UNICAST_HOPS));
	Nan::Set(socket_option, Nan::New("IPV6_UNICAST_HOPS").ToLocalChecked(), Nan::New<Number>(IPV6_UNICAST_HOPS));
	Nan::Set(socket_option, Nan::New("IPV6_V6ONLY").ToLocalChecked(), Nan::New<Number>(IPV6_V6ONLY));
}

void ExportFunctions (Local<Object> target) {
	Nan::Set(target, Nan::New("createChecksum").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CreateChecksum)).ToLocalChecked());
	
	Nan::Set(target, Nan::New("htonl").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(Htonl)).ToLocalChecked());
	Nan::Set(target, Nan::New("htons").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(Htons)).ToLocalChecked());
	Nan::Set(target, Nan::New("ntohl").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(Ntohl)).ToLocalChecked());
	Nan::Set(target, Nan::New("ntohs").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(Ntohs)).ToLocalChecked());
}

void SocketWrap::Init (Local<Object> exports) {
	Nan::HandleScope scope;

	Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(SocketWrap::New);
	tpl->SetClassName(Nan::New("SocketWrap").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	Nan::SetPrototypeMethod(tpl, "close", Close);
	Nan::SetPrototypeMethod(tpl, "getOption", GetOption);
	Nan::SetPrototypeMethod(tpl, "pause", Pause);
	Nan::SetPrototypeMethod(tpl, "recv", Recv);
	Nan::SetPrototypeMethod(tpl, "send", Send);
	Nan::SetPrototypeMethod(tpl, "setOption", SetOption);

	SocketWrap_constructor.Reset(tpl);
	Nan::Set(exports, Nan::New("SocketWrap").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

SocketWrap::SocketWrap () {
	deconstructing_ = false;
}

SocketWrap::~SocketWrap () {
	deconstructing_ = true;
	this->CloseSocket ();
}

NAN_METHOD(SocketWrap::Close) {
	Nan::HandleScope scope;
	
	SocketWrap* socket = SocketWrap::Unwrap<SocketWrap> (info.This ());
	
	socket->CloseSocket ();

	Local<Value> args[1];
	args[0] = Nan::New<String>("close").ToLocalChecked();

	Nan::Call(Nan::New<String>("emit").ToLocalChecked(), info.This(), 1, args);

	info.GetReturnValue().Set(info.This());
}

void SocketWrap::CloseSocket (void) {
	if (this->poll_initialised_) {
		uv_close ((uv_handle_t *) this->poll_watcher_, OnClose);
		closesocket (this->poll_fd_);
		this->poll_fd_ = INVALID_SOCKET;
		this->poll_initialised_ = false;
	}
}

int SocketWrap::CreateSocket (void) {
	if (this->poll_initialised_)
		return 0;
	
	/*Kihei: Open AF_PACKET socket, listening for EtherType ETHER_TYPE=0x88dc */
	this->poll_fd_ = socket (AF_PACKET, SOCK_RAW, htons(ETHER_TYPE));
	//this->poll_fd_ = socket (this->family_, SOCK_RAW, this->protocol_);
	
#ifdef __APPLE__
	/**
	 ** On MAC OS X platforms for non-privileged users wishing to utilise ICMP
	 ** a SOCK_DGRAM will be enough, so try to create this type of socket in
	 ** the case ICMP was requested.
	 **
	 ** More information can be found at:
	 **
	 **  https://developer.apple.com/library/mac/documentation/Darwin/Reference/Manpages/man4/icmp.4.html
	 **
	 **/
	if (this->poll_fd_ == INVALID_SOCKET && this->protocol_ == IPPROTO_ICMP)
		this->poll_fd_ = socket (this->family_, SOCK_DGRAM, this->protocol_);
#endif

	if (this->poll_fd_ == INVALID_SOCKET)
		return SOCKET_ERRNO;

#ifdef _WIN32
	unsigned long flag = 1;
	if (ioctlsocket (this->poll_fd_, FIONBIO, &flag) == SOCKET_ERROR)
		return SOCKET_ERRNO;
#else
	int flag = 1;
	if ((flag = fcntl (this->poll_fd_, F_GETFL, 0)) == SOCKET_ERROR)
		return SOCKET_ERRNO;
	if (fcntl (this->poll_fd_, F_SETFL, flag | O_NONBLOCK) == SOCKET_ERROR)
		return SOCKET_ERRNO;
#endif

	poll_watcher_ = new uv_poll_t;
	uv_poll_init_socket (uv_default_loop (), this->poll_watcher_,
			this->poll_fd_);
	this->poll_watcher_->data = this;
	uv_poll_start (this->poll_watcher_, UV_READABLE, IoEvent);
	
	this->poll_initialised_ = true;
	
	return 0;
}

NAN_METHOD(SocketWrap::GetOption) {
	Nan::HandleScope scope;
	
	SocketWrap* socket = SocketWrap::Unwrap<SocketWrap> (info.This ());
	
	if (info.Length () < 3) {
		Nan::ThrowError("Three arguments are required");
		return;
	}

	if (! info[0]->IsNumber ()) {
		Nan::ThrowTypeError("Level argument must be a number");
		return;
	}

	if (! info[1]->IsNumber ()) {
		Nan::ThrowTypeError("Option argument must be a number");
		return;
	}

	int level = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	int option = Nan::To<Uint32>(info[1]).ToLocalChecked()->Value();
	SOCKET_OPT_TYPE val = NULL;
	unsigned int ival = 0;
	SOCKET_LEN_TYPE len;

	if (! node::Buffer::HasInstance (info[2])) {
		Nan::ThrowTypeError("Value argument must be a node Buffer object if length is provided");
		return;
	}
	
	Local<Object> buffer = Nan::To<Object>(info[2]).ToLocalChecked();
	val = node::Buffer::Data (buffer);

	if (! info[3]->IsInt32 ()) {
		Nan::ThrowTypeError("Length argument must be an unsigned integer");
		return;
	}

	len = (SOCKET_LEN_TYPE) node::Buffer::Length (buffer);

	int rc = getsockopt (socket->poll_fd_, level, option,
			(val ? val : (SOCKET_OPT_TYPE) &ival), &len);

	if (rc == SOCKET_ERROR) {
		Nan::ThrowError(raw_strerror (SOCKET_ERRNO));
		return;
	}
	
	Local<Number> got = Nan::New<Uint32>(len);
	
	info.GetReturnValue().Set(got);
}

void SocketWrap::HandleIOEvent (int status, int revents) {
	Nan::HandleScope scope;

	if (status) {
		Local<Value> args[2];
		args[0] = Nan::New<String>("error").ToLocalChecked();
		
		/**
		 ** The uv_last_error() function doesn't seem to be available in recent
		 ** libuv versions, and the uv_err_t variable also no longer appears to
		 ** be a structure.  This causes issues when working with both Node.js
		 ** 0.10 and 0.12.  So, for now, we will just give you the number.
		 **/
		char status_str[32];
		sprintf(status_str, "%d", status);
		args[1] = Nan::Error(status_str);

		Nan::Call(Nan::New<String>("emit").ToLocalChecked(), handle(), 1, args);
	} else {
		Local<Value> args[1];
		if (revents & UV_READABLE)
			args[0] = Nan::New<String>("recvReady").ToLocalChecked();
		else
			args[0] = Nan::New<String>("sendReady").ToLocalChecked();

		Nan::Call(Nan::New<String>("emit").ToLocalChecked(), handle(), 1, args);
	}
}

NAN_METHOD(SocketWrap::New) {
	Nan::HandleScope scope;
	
	SocketWrap* socket = new SocketWrap ();
	int rc, family = AF_PACKET;//AF_INET; // AF_INET throws a Protocol not supported error.
	
	if (info.Length () < 1) {
		Nan::ThrowError("One argument is required");
		return;
	}
	
	if (! info[0]->IsUint32 ()) {
		Nan::ThrowTypeError("Protocol argument must be an unsigned integer");
		return;
	} else {
		socket->protocol_ = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	}

	if (info.Length () > 1) {
		if (! info[1]->IsUint32 ()) {
			Nan::ThrowTypeError("Address family argument must be an unsigned integer");
			return;
		} else {
			if (Nan::To<Uint32>(info[1]).ToLocalChecked()->Value() == 2)
				family = AF_INET6;
		}
	}
	
	
	socket->family_ = family;
	
	socket->poll_initialised_ = false;
	
	socket->no_ip_header_ = false;

	rc = socket->CreateSocket ();
	if (rc != 0) {
		Nan::ThrowError(raw_strerror (rc));
		return;
	}

	socket->Wrap (info.This ());

	info.GetReturnValue().Set(info.This());
}

void SocketWrap::OnClose (uv_handle_t *handle) {
	delete handle;
}

NAN_METHOD(SocketWrap::Pause) {
	Nan::HandleScope scope;
	
	SocketWrap* socket = SocketWrap::Unwrap<SocketWrap> (info.This ());

	if (info.Length () < 2) {
		Nan::ThrowError("Two arguments are required");
		return;
	}
	
	if (! info[0]->IsBoolean ()) {
		Nan::ThrowTypeError("Recv argument must be a boolean");
		return;
	}
	bool pause_recv = Nan::To<Boolean>(info[0]).ToLocalChecked()->Value();

	if (! info[1]->IsBoolean ()) {
		Nan::ThrowTypeError("Send argument must be a boolean");
		return;
	}
	bool pause_send = Nan::To<Boolean>(info[1]).ToLocalChecked()->Value();
	
	int events = (pause_recv ? 0 : UV_READABLE)
			| (pause_send ? 0 : UV_WRITABLE);

	if (! socket->deconstructing_ && socket->poll_initialised_) {
		uv_poll_stop (socket->poll_watcher_);
		if (events)
			uv_poll_start (socket->poll_watcher_, events, IoEvent);
	}
	
	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(SocketWrap::Recv) {
	Nan::HandleScope scope;
//cout<<"recv\n";	
	SocketWrap* socket = SocketWrap::Unwrap<SocketWrap> (info.This ());
	Local<Object> buffer;
	sockaddr_in sin_address;
	sockaddr_in6 sin6_address;
	char addr[50];
	int rc;
#ifdef _WIN32
	int sin_length = socket->family_ == AF_INET6
			? sizeof (sin6_address)
			: sizeof (sin_address);
#else
	socklen_t sin_length = socket->family_ == AF_INET6
			? sizeof (sin6_address)
			: sizeof (sin_address);
#endif
	
	if (info.Length () < 2) {
		Nan::ThrowError("Five arguments are required");
		return;
	}
	
	if (! node::Buffer::HasInstance (info[0])) {
		Nan::ThrowTypeError("Buffer argument must be a node Buffer object");
		return;
	} else {
		buffer = Nan::To<Object>(info[0]).ToLocalChecked();
	}

	if (! info[1]->IsFunction ()) {
		Nan::ThrowTypeError("Callback argument must be a function");
		return;
	}

	rc = socket->CreateSocket ();
	if (rc != 0) {
		Nan::ThrowError(raw_strerror (errno));
		return;
	}

	if (socket->family_ == AF_INET6) {
		memset (&sin6_address, 0, sizeof (sin6_address));
		rc = recvfrom (socket->poll_fd_, node::Buffer::Data (buffer),
				(int) node::Buffer::Length (buffer), 0, (sockaddr *) &sin6_address,
				&sin_length);
	} else {
		memset (&sin_address, 0, sizeof (sin_address));
		rc = recvfrom (socket->poll_fd_, node::Buffer::Data (buffer),
				(int) node::Buffer::Length (buffer), 0, (sockaddr *) &sin_address,
				&sin_length);
	}
	
	if (rc == SOCKET_ERROR) {
		Nan::ThrowError(raw_strerror (SOCKET_ERRNO));
		return;
	}
	
	if (socket->family_ == AF_INET6)
		uv_ip6_name (&sin6_address, addr, 50);
	else
		uv_ip4_name (&sin_address, addr, 50);
	
	Local<Function> cb = Local<Function>::Cast (info[1]);
	const unsigned argc = 3;
	Local<Value> argv[argc];
	argv[0] = info[0];
	argv[1] = Nan::New<Number>(rc);
	argv[2] = Nan::New(addr).ToLocalChecked();
	Nan::Call(Nan::Callback(cb), argc, argv);
	
	info.GetReturnValue().Set(info.This());
}

NAN_METHOD(SocketWrap::Send) {
	Nan::HandleScope scope;
	uint8_t sendbuf[100];
	int tx_len = 0;
	SocketWrap* socket = SocketWrap::Unwrap<SocketWrap> (info.This ());
	Local<Object> buffer;
	uint32_t offset;
	uint32_t length;
	int rc;
	char *data;
	
	if (info.Length () < 5) {
		Nan::ThrowError("Five arguments are required");
		return;
	}
	
	if (! node::Buffer::HasInstance (info[0])) {
		Nan::ThrowTypeError("Buffer argument must be a node Buffer object");
		return;
	}
	
	if (! info[1]->IsUint32 ()) {
		Nan::ThrowTypeError("Offset argument must be an unsigned integer");
		return;
	}

	if (! info[2]->IsUint32 ()) {
		Nan::ThrowTypeError("Length argument must be an unsigned integer");
		return;
	}

	if (! info[3]->IsString ()) {
		Nan::ThrowTypeError("Address argument must be a string");
		return;
	}

	if (! info[4]->IsFunction ()) {
		Nan::ThrowTypeError("Callback argument must be a function");
		return;
	}

	rc = socket->CreateSocket ();
	if (rc != 0) {
//cout<<"raw.cc send rc!=0\n";
		Nan::ThrowError(raw_strerror (errno));
		return;
	}
	
	buffer = Nan::To<Object>(info[0]).ToLocalChecked();
	offset = Nan::To<Uint32>(info[1]).ToLocalChecked()->Value();
	length = Nan::To<Uint32>(info[2]).ToLocalChecked()->Value();

	data = node::Buffer::Data (buffer) + offset;
	
	if (socket->family_ == AF_INET6) {
#if UV_VERSION_MAJOR > 0
		struct sockaddr_in6 addr;

		uv_ip6_addr(*Nan::Utf8String(info[3]), 0, &addr);
#else
		String::Utf8String address (args[3]);
		struct sockaddr_in6 addr = uv_ip6_addr (*address, 0);
#endif
		
		rc = sendto (socket->poll_fd_, data, length, 0,
				(struct sockaddr *) &addr, sizeof (addr));
//cout<<"raw.cc send socket->family_ == AF_INET6\n";
	} else {
#if UV_VERSION_MAJOR > 0
		struct sockaddr_in addr;
		uv_ip4_addr(*Nan::Utf8String(info[3]), 0, &addr);
#else
		String::Utf8String address (info[3]);
		struct sockaddr_in addr = uv_ip4_addr (*address, 0);
#endif
		//cout<<"raw.cc send sockaddr_in\n";
	//	const unsigned char ether_broadcast_addr[] = {0xff,0xff,0xff,0xff,0xff,0xff};
	//memcpy(addr.sll_addr,ether_broadcast_addr,ETHER_ADDR_LEN);


char ifName[16]; 
strcpy(ifName, DEFAULT_IF); 
	struct ifreq if_mac2;
	struct ifreq if_idx2; 
	memset(&if_idx2, 0, sizeof(struct ifreq));
	strncpy(if_idx2.ifr_name, ifName, 16-1);
	//cout<<if_idx2.ifr_name<<"\n";
	memset(&if_mac2, 0, sizeof(struct ifreq));
	strncpy(if_mac2.ifr_name, ifName, 16-1);
	//char sendbuf[1024];
	/*struct ether_header *eh = (struct ether_header *) sendbuf;
	/* Ethernet header 
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	eh->ether_type = htons(ETHER_TYPE);*/
	
char mac[6]={0};
getMacAddress(mac);
struct sockaddr_ll addr_ll={0}; 
addr_ll.sll_ifindex = if_nametoindex(ifName);//if_idx2.ifr_ifindex;
//cout << if_idx2.ifr_ifindex << endl;
addr_ll.sll_halen=6; 
addr_ll.sll_protocol=htons(ETHER_TYPE); 
addr_ll.sll_family = AF_PACKET;
addr_ll.sll_addr[0] = mac[0];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[0];
	addr_ll.sll_addr[1] = mac[1];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[1];
	addr_ll.sll_addr[2] = mac[2];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[2];
	addr_ll.sll_addr[3] = mac[3];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[3];
	addr_ll.sll_addr[4] = mac[4];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[4];
	addr_ll.sll_addr[5] = mac[5];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[5];
//printf("\n%02X:%02X:%02X:%02X:%02X:%02X\n", addr_ll.sll_addr[0], addr_ll.sll_addr[1], addr_ll.sll_addr[2], addr_ll.sll_addr[3], addr_ll.sll_addr[4], addr_ll.sll_addr[5]);

 	const unsigned char ether_broadcast_addr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
memcpy(addr_ll.sll_addr,ether_broadcast_addr,6);

//printf("\n%02X:%02X:%02X:%02X:%02X:%02X\n", ether_broadcast_addr[0], ether_broadcast_addr[1], ether_broadcast_addr[2], ether_broadcast_addr[3], ether_broadcast_addr[4], ether_broadcast_addr[5]);

//cout << data << endl;
//cout << hex << ether_broadcast_addr[0] << ":" << hex << ether_broadcast_addr[1] <<endl;
//printf("\nContents of a structure %06X are:\n", *ether_broadcast_addr);
	//rc = sendto (socket->poll_fd_, data, length, 0,(struct sockaddr *) &addr_ll, sizeof (addr_ll));



//length = 77;


//DEST MAC
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
//SOURCE MAC
sendbuf[tx_len++] = mac[0];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[0];
sendbuf[tx_len++] = mac[1];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[1];
sendbuf[tx_len++] = mac[2];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[2];
sendbuf[tx_len++] = mac[3];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[3];
sendbuf[tx_len++] = mac[4];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[4];
sendbuf[tx_len++] = mac[5];//((uint8_t *)&if_mac2.ifr_hwaddr.sa_data)[5];
//WSMP
sendbuf[tx_len++] = 0x88;
sendbuf[tx_len++] = 0xdc;
sendbuf[tx_len++] = 0xaa;
sendbuf[tx_len++] = 0xbb;
sendbuf[tx_len++] = 0xcc;
//DATA

int k=0;	
for ( ; k < length; k++ )
{
   sendbuf[tx_len++] = data[k];
}

/*sendbuf[tx_len++] = 0x02;
sendbuf[tx_len++] = 0x20;
sendbuf[tx_len++] = 0x0f;
sendbuf[tx_len++] = 0x01;
sendbuf[tx_len++] = 0xac;
sendbuf[tx_len++] = 0x10;
sendbuf[tx_len++] = 0x01;
sendbuf[tx_len++] = 0x0c;
sendbuf[tx_len++] = 0x04;
sendbuf[tx_len++] = 0x01;
sendbuf[tx_len++] = 0x01;
sendbuf[tx_len++] = 0x80;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x31;
sendbuf[tx_len++] = 0x02;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x30;
sendbuf[tx_len++] = 0x2d;
sendbuf[tx_len++] = 0x80;
sendbuf[tx_len++] = 0x01;
sendbuf[tx_len++] = 0x02;
sendbuf[tx_len++] = 0x81;
sendbuf[tx_len++] = 0x26;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x11;
sendbuf[tx_len++] = 0x22;
sendbuf[tx_len++] = 0x33;
sendbuf[tx_len++] = 0x44;
sendbuf[tx_len++] = 0x87;
sendbuf[tx_len++] = 0x28;
sendbuf[tx_len++] = 0x35;
sendbuf[tx_len++] = 0xa4;
sendbuf[tx_len++] = 0xe9;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x5f;
sendbuf[tx_len++] = 0x5e;
sendbuf[tx_len++] = 0xe1;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0x07;
sendbuf[tx_len++] = 0xd1;
sendbuf[tx_len++] = 0x81;
sendbuf[tx_len++] = 0x7f;
sendbuf[tx_len++] = 0xff;
sendbuf[tx_len++] = 0x08;
sendbuf[tx_len++] = 0x00;
sendbuf[tx_len++] = 0x07;
sendbuf[tx_len++] = 0x80;
sendbuf[tx_len++] = 0x1e;
sendbuf[tx_len++] = 0xa3;
sendbuf[tx_len++] = 0x00;*/


//cout<<endl<<"Mac Address : "<<mac;
	
int i=0;	
for ( ; i < tx_len; i++ )
{
   printf("0x%02x, ", sendbuf[i]);
}
printf("\n");
rc = sendto (socket->poll_fd_, sendbuf, tx_len, 0,(struct sockaddr *) &addr_ll, sizeof (addr_ll));

	}
//cout<<"raw.cc send\n";	
	if (rc == SOCKET_ERROR) {
		cout << tx_len << "\n";
		cout << sendbuf << "\n";
        cout<<"raw.cc SocketWrap::Send\n";
		Nan::ThrowError(raw_strerror (SOCKET_ERRNO));
		return;
	}
	
	Local<Function> cb = Local<Function>::Cast (info[4]);
	const unsigned argc = 1;
	Local<Value> argv[argc];
	argv[0] = Nan::New<Number>(rc);
	Nan::Call(Nan::Callback(cb), argc, argv);
	
	info.GetReturnValue().Set(info.This());
}

void SocketWrap::getMacAddress(char * uc_Mac)
{
   	 int fd;
	
	struct ifreq ifr;
	char *iface = DEFAULT_IF;
	char *mac;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy((char *)ifr.ifr_name , (const char *)iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);
	
	mac = (char *)ifr.ifr_hwaddr.sa_data;
	uc_Mac[0]  = mac[0];
	uc_Mac[1]  = mac[1];
	uc_Mac[2]  = mac[2];
	uc_Mac[3]  = mac[3];
	uc_Mac[4]  = mac[4];
	uc_Mac[5]  = mac[5];
	//display mac address
	//sprintf((char *)uc_Mac,(const char *)"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	
}

NAN_METHOD(SocketWrap::SetOption) {
	Nan::HandleScope scope;
	
	SocketWrap* socket = SocketWrap::Unwrap<SocketWrap> (info.This ());
	
	if (info.Length () < 3) {
		Nan::ThrowError("Three or four arguments are required");
		return;
	}

	if (! info[0]->IsNumber ()) {
		Nan::ThrowTypeError("Level argument must be a number");
		return;
	}

	if (! info[1]->IsNumber ()) {
		Nan::ThrowTypeError("Option argument must be a number");
		return;
	}

	int level = Nan::To<Uint32>(info[0]).ToLocalChecked()->Value();
	int option = Nan::To<Uint32>(info[1]).ToLocalChecked()->Value();
	SOCKET_OPT_TYPE val = NULL;
	unsigned int ival = 0;
	SOCKET_LEN_TYPE len;

	if (info.Length () > 3) {
		if (! node::Buffer::HasInstance (info[2])) {
			Nan::ThrowTypeError("Value argument must be a node Buffer object if length is provided");
			return;
		}
		
		Local<Object> buffer = Nan::To<Object>(info[2]).ToLocalChecked();
		val = node::Buffer::Data (buffer);

		if (! info[3]->IsInt32 ()) {
			Nan::ThrowTypeError("Length argument must be an unsigned integer");
			return;
		}

		len = Nan::To<Uint32>(info[3]).ToLocalChecked()->Value();

		if (len > node::Buffer::Length (buffer)) {
			Nan::ThrowTypeError("Length argument is larger than buffer length");
			return;
		}
	} else {
		if (! info[2]->IsUint32 ()) {
			Nan::ThrowTypeError("Value argument must be a unsigned integer");
			return;
		}

		ival = Nan::To<Uint32>(info[2]).ToLocalChecked()->Value();
		len = 4;
	}

	/*Kihei: Bind to device*/
	char ifName[16];
	strcpy(ifName, DEFAULT_IF);
	int rc = setsockopt(socket->poll_fd_, SOL_SOCKET, SO_BINDTODEVICE, ifName, 16-1);
	//int rc = setsockopt (socket->poll_fd_, level, option,
		//	(val ? val : (SOCKET_OPT_TYPE) &ival), len);

	if (rc == SOCKET_ERROR) {
		Nan::ThrowError(raw_strerror(SOCKET_ERRNO));
		return;
	}
	
	info.GetReturnValue().Set(info.This());
}

static void IoEvent (uv_poll_t* watcher, int status, int revents) {
	SocketWrap *socket = static_cast<SocketWrap*>(watcher->data);
	socket->HandleIOEvent (status, revents);
}

}; /* namespace raw */

#endif /* RAW_CC */
