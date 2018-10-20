#include "subd.h"

DBusConnection *subd_open_session(const char* service_name, DBusError *err) {
	if (!dbus_threads_init_default()) {
		dbus_set_error(err, DBUS_ERROR_NO_MEMORY, NULL);
		return NULL;
	}

	DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, err);
	if (connection == NULL) {
		return NULL;
	}

	if (!dbus_bus_request_name(connection, service_name, 0, err)) {
		dbus_connection_unref(connection);
		return NULL;
	}

	return connection;
}

dbus_bool_t subd_emit_signal(DBusConnection *conn, const char *path,
		const char *interface, const char *name, DBusError *err, ...) {
	DBusMessage *signal = NULL;
	if ((signal = dbus_message_new_signal(path, interface, name)) == NULL) {
		goto error;
	}

	va_list ap;
	va_start(ap, err);
	if (!dbus_message_append_args_valist(signal, va_arg(ap, int), ap)) {
		goto error;
	}
	va_end(ap);

	if (!dbus_connection_send(conn, signal, NULL)) {
		goto error;
	}

	dbus_message_unref(signal);
	return TRUE;

error:
	dbus_set_error(err, DBUS_ERROR_NO_MEMORY, NULL);
	dbus_message_unref(signal);
	return FALSE;
}

dbus_bool_t subd_reply_method_return(DBusConnection *conn, DBusMessage *msg,
		DBusError *err, ...) {
	DBusMessage *reply = NULL;
	if ((reply = dbus_message_new_method_return(msg)) == NULL) {
		goto error;
	}

	va_list ap;
	va_start(ap, err);
	if (!dbus_message_append_args_valist(reply, va_arg(ap, int), ap)) {
		goto error;
	}
	va_end(ap);

	if (!dbus_connection_send(conn, reply, NULL)) {
		goto error;
	}

	dbus_message_unref(reply);
	return TRUE;

error:
	dbus_set_error(err, DBUS_ERROR_NO_MEMORY, NULL);
	dbus_message_unref(reply);
	return FALSE;
}

dbus_bool_t subd_message_read(DBusMessageIter *iter, DBusError *err, ...) {
	va_list ap;
	va_start(ap, err);
	DBusBasicValue *ptr = NULL;
	while ((ptr = va_arg(ap, DBusBasicValue *)) != NULL) {
		dbus_message_iter_get_basic(iter, ptr);
		if (!dbus_message_iter_next(iter)) {
			dbus_set_error(err, DBUS_ERROR_INVALID_ARGS,
				"Message has less fields than requested");
			return FALSE;
		}
	}
	va_end(ap);

	return TRUE;
}
