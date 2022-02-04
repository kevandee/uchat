#include "../inc/uch_server.h"

SSL_CTX* CTX_initialize_server() {
    const SSL_METHOD *ssl_method;
    SSL_CTX *ssl_context; 

    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ssl_method = TLS_server_method();
    ssl_context = SSL_CTX_new(ssl_method);

    if(ssl_context == NULL) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ssl_context;
}

EVP_PKEY *create_key() {
    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        perror("ERROR: Could not create private key!(1)\n");
        abort();
    }

    RSA *rsa = RSA_new();
    //RSA_generate_key(1024, RSA_F4, NULL, NULL);
    BIGNUM *bignumb = BN_new();
    BN_set_word(bignumb, 65537);
    RSA_generate_key_ex(rsa, 1024, bignumb, NULL);
    BN_free(bignumb);
    if (rsa == NULL) {
        perror("ERROR: Could not create RSA key!(2)\n");
        abort();
    }

    if (!EVP_PKEY_assign_RSA(pkey, rsa)) {
        perror("ERROR: Could not assign RSA key!(3)\n");
        abort();
    }

    return pkey;
}

X509 *create_X509(EVP_PKEY *pkey) {
    X509 *x509 = X509_new();
    if (x509 == NULL) {
        perror("ERROR: Could not create X509 certificate!(4)\n");
        perror(ERR_error_string(ERR_get_error(), NULL));
        abort();
    }

    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    //set span of time during which cert is valid
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);
    //set publick key to our sert using priv key
    X509_set_pubkey(x509, pkey);

    X509_NAME * name;
    name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, (unsigned char *)"UA", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, (unsigned char *)"Glynomesy Inc.", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"Kukoldinio DimASS de bebrou", -1, -1, 0);

    X509_set_issuer_name(x509, name);
    if (!X509_sign(x509, pkey, EVP_sha1())) {
        perror("ERROR: Could not sign X509 certificate!(5)\n");
        abort();
    }

    return x509;
}

void write_certs(EVP_PKEY *pkey, X509 *x509) {
    FILE *f_key;
    f_key = fopen("PEM_privatekey.pem", "wb");
    if(!f_key) {
        perror("ERROR: Could not open key pem file!(1)\n");
        abort();
    }
    if (!PEM_write_PrivateKey(f_key, pkey, NULL, NULL, 0, NULL, NULL)) {
        perror("ERROR: Could not write key pem file!(2)\n");
    }
    fclose(f_key);

    FILE *f_cert;
    f_cert = fopen("certificate.pem", "wb");
    if(!f_cert) {
        perror("ERROR: Could not open certificate pem file!(3)\n");
        abort();
    }
    if (!PEM_write_X509(f_cert, x509)) {
        perror("ERROR: Could not write certificate pem file!(2)\n");
    }   
    fclose(f_cert);
}

void load_certs(SSL_CTX* context, char* cert_name, char* key_name) {
    // set the local certificate from file with certificate
    if ( SSL_CTX_use_certificate_file(context, cert_name, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    // set the private key from key_name (may be the same as cert_name)
    if ( SSL_CTX_use_PrivateKey_file(context, key_name, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    // verify private key
    if ( !SSL_CTX_check_private_key(context) )
    {
        fprintf(stderr, "ERROR: Private key does not match the public certificate\n");
        abort();
    }
}

int open_server_connection(int port, struct sockaddr_in *adr, socklen_t adrlen) {
    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);

    (*adr).sin_family = AF_INET;
    (*adr).sin_port = htons(port);
    (*adr).sin_addr.s_addr = INADDR_ANY;

    if (bind(serv_fd, (struct sockaddr*)&(*adr), adrlen) < 0 ) {
        perror("ERROR: Socket binding failed\n");
        abort();
    }
    if ( listen(serv_fd, 5) < 0) {
        perror("ERROR: Can't configure listening port\n");
        abort();
    }

    return serv_fd;
}

// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
// Get peer certificate (clients) and outputs info about cert owner
// but here client has no certificate. Better keep it for later
// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
void certificate_ckeck(SSL *ssl) {
    X509 *cert = SSL_get_peer_certificate(ssl);
    if ( cert != NULL ) {
        printf("SSL: server certificates:\n");
        char *line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("SSL: subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("SSL: issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else {
        printf("No certificates\n");
    }
}

// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
// Need to close conection and free all
// the allocated ssl memory in the end 
// of clients work and if abort occurs
// VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
//      UNFINISHED
void close_connection(SSL *ssl) {
    int socket = SSL_get_fd(ssl);
    SSL_free(ssl);
    close(socket);
}

void close_server(EVP_PKEY *pkey, X509 *x509, SSL_CTX *context) {
    EVP_PKEY_free(pkey);
    X509_free(x509);
    SSL_CTX_free(context);
}
