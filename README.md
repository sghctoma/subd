# subd

**WARNING**: this is really WIP, anything could change, please don't use it yet!

Subd is a collection of helper functions that make using dbus-1 more convenient.
For me, at least, but I hope others will find it useful as well :) It is by no
means a complete wrapper around libdbus, it just provides some functions that
make using the low-level API easier. This means that you still have to use the
low-level API with subd - as I have stated earlier, this is just a collection of
helper functions.

The reason I wrote these functions is that I started porting an application that
uses sd-bus. We don't have sd-bus on FreeBSD (and probably never will), but I
really liked some aspects of it (especially the way it lets you define object
members, and method handlers). I also didn't want to use a complete DBus library
like GDBus or QTDBus, so subd was born.

The scope is very limited currently, it basically provides helpers only in areas
I needed to work with so far:

 * Opening a connection to the bus.
 * Reading and sending messages.
 * Implementing the Introspectable interface.
 * Dispatching handlers for method type members.
 * Handling watches.

## subd API

### Miscellaneous functions

```c
DBusConnection *subd_open_session(const char *service_name, DBusError *err);

dbus_bool_t subd_emit_signal(DBusConnection *conn, const char *path,
	const char *interface, const char *name, DBusError *err,  ...);

dbus_bool_t subd_reply_method_return(DBusConnection *conn, DBusMessage *msg,
	DBusError *err, ...);

dbus_bool_t subd_message_read(DBusMessageIter *iter, DBusError *err, ...);
```

### Functions and data structures that deal with watches

```c
struct subd_watches {
	struct pollfd *fds;
	DBusWatch **watches;
	int capacity;	
	int length;
	sem_t mutex;
};

struct subd_watches *subd_init_watches(DBusConnection *conn, struct pollfd *fds,
	int size, DBusError *err);
```

### Functions and data structures that deal with interface implementation

```c
enum subd_member_type {
	SUBD_METHOD,
	SUBD_SIGNAL,
	SUBD_PROPERTY,
	SUBD_MEMBERS_END,
};

enum subd_property_access {
	SUBD_PROPERTY_READ,
	SUBD_PROPERTY_WRITE,
	SUBD_PROPERTY_READWRITE,
};

struct subd_member {
	enum subd_member_type type;
	union {
		struct {
			const char *name;
			dbus_bool_t (*handler)(DBusConnection *, DBusMessage *, void *, DBusError *);
			const char *input_signature;
			const char *output_signature;
		} m;
		struct {
			const char *name;
			const char *signature;
		} s;
		struct {
			const char *name;
			const char *signature;
			enum subd_property_access access;
		} p;
		int e;
	};
};

dbus_bool_t subd_add_object_vtable(DBusConnection *conn, const char *path,
	const char *interface, const struct subd_member *members, void *userdata,
	DBusError *err);
```

For a detailed description of what each function does, please refer to the
include/subd.h file.
