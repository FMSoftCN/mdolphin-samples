#ifndef _MDOLPHIN_CERTIFICATE_H
#define _MDOLPHIN_CERTIFICATE_H
#include <mdolphin/mdolphin.h>

#if ENABLE_SSL
#include "openssl/x509.h"
#if ENABLE_SSLFILE
BOOL readCertificateHome(unsigned long hash, X509 **x509, EVP_PKEY **pkey);
BOOL importCertificate(HWND hParent);
BOOL importServerCertificate(HWND hParent);

int saveCertificateHome(X509 *cert, bool isClient);
BOOL  setVerifyCAPath();
#endif
int verifyCertificate(HWND hParent, MD_CERT_RESULT result, X509 *x509);
#endif
#endif
