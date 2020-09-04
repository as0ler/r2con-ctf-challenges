/* 
    r2con2020 CTF - Cyber Lock
    @hexploitable

    Challenge description:
    1. Only accept input at the third minute of every hour. - expected to patch this
    2. Input via file the code below, it will then say how many pins are correct.
    3. If all pins are correct, use the input to derive key to decrypt flag
*/

// Imports
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdint.h>
#include <assert.h>

/* defines */
#define kRED  "\x1B[31m"
#define kGRN  "\x1B[32m"
#define kRST  "\033[0m"
#define AES_256_KEY_LENGTH      32
#define AES_256_KEY_LENGTH_BITS 256
#define AES_256_IVEC_LENGTH     12
#define AES_256_GCM_TAG_LENGTH  16

/* 
    sample input
    12 sha256 "pins", each a single ASCII character.
    without quotes: "are you real".
*/
const char *code = "\
2E7D2C03A9507AE265ECF5B5356885A53393A2029D241394997265A1A25AEFC6\
454349E422F05297191EAD13E21D3DB520E5ABEF52055E4964B82FB213F593A1\
3F79BB7B435B05321651DAEFD374CDC681DC06FAA65E374E38337B88CA046DEA\
36A9E7F1C95B82FFB99743E0C5C4CE95D83C9A430AAC59F84EF3CBFAB6145068\
A1FCE4363854FF888CFF4B8E7875D600C2682390412A8CF79B37D0B11148B0FA\
65C74C15A686187BB6BBF9958F494FC6B80068034A659A9AD44991B08C58F2D2\
0BFE935E70C321C7CA3AFC75CE0D0CA2F98B5422E008BB31C00C6D7F1F1C0AD6\
36A9E7F1C95B82FFB99743E0C5C4CE95D83C9A430AAC59F84EF3CBFAB6145068\
454349E422F05297191EAD13E21D3DB520E5ABEF52055E4964B82FB213F593A1\
3F79BB7B435B05321651DAEFD374CDC681DC06FAA65E374E38337B88CA046DEA\
CA978112CA1BBDCAFAC231B39A23DC4DA786EFF8147C4E72B9807785AFEE48BB\
ACAC86C0E609CA906F632B0E2DACCCB2B77D22B0621F20EBECE1A4835B93F6F0";

static void printLock () {
    printf("[i] R2conCTF 2020 - CyberLock 1.3.3.7-alpha\n");
    printf("\n");
    printf("     .--------. \n");
    printf("    / .------. \\\n");
    printf("   / /        \\ \\\n");
    printf("   | |        | | \n");
    printf("  _| |________| |_ \n");
    printf(".' |_|        |_| '. \n");
    printf("'._____ ____ _____.' \n");
    printf("|     .'____'.     | \n");
    printf("'.__.'.'    '.'.__.' \n");
    printf("'.__  |!R2Con|  __.' \n");
    printf("|   '.'.____.'.'   | \n");
    printf("'.____'.____.'____.' \n");
    printf("'.________________.' \n");
    printf("\n");

    printf("[i] The r2con Cyberlock is the world's only quantum, military-grade, agile, blockchain backed, hardened, bullet-proof 12-pin digital cryptographic cyber lock.\n");
    printf("[i] Hackers will never make it past the first pin!!!!\n\n");
}

/*
    input is crafted as: {sha256 of pin}_{minute (3)}_{r2con2020}.
    input is hashed using sha512 and compared with the {hashes} array per the correct pin index.
    pin1:   c    ... generated str: 2E7D2C03A9507AE265ECF5B5356885A53393A2029D241394997265A1A25AEFC6_3_r2con2020
    pin2:   h    ... generated str: AAA9402664F1A41F40EBBC52C9993EB66AEB366602958FDFAA283B71E64DB123_3_r2con2020
    pin3:   a    ... generated str: CA978112CA1BBDCAFAC231B39A23DC4DA786EFF8147C4E72B9807785AFEE48BB_3_r2con2020
    pin4:   o    ... generated str: 65C74C15A686187BB6BBF9958F494FC6B80068034A659A9AD44991B08C58F2D2_3_r2con2020
    pin5:   s    ... generated str: 043A718774C572BD8A25ADBEB1BFCD5C0256AE11CECF9F9C3F925D0E52BEAF89_3_r2con2020
    pin6:   M    ... generated str: 08F271887CE94707DA822D5263BAE19D5519CB3614E0DAEDC4C7CE5DAB7473F1_3_r2con2020
    pin7:   o    ... generated str: 65C74C15A686187BB6BBF9958F494FC6B80068034A659A9AD44991B08C58F2D2_3_r2con2020
    pin8:   N    ... generated str: 8CE86A6AE65D3692E7305E2C58AC62EEBD97D3D943E093F577DA25C36988246B_3_r2con2020
    pin9:   K    ... generated str: 86BE9A55762D316A3026C2836D044F5FC76E34DA10E1B45FEEE5F18BE7EDB177_3_r2con2020
    pin10:  e    ... generated str: 3F79BB7B435B05321651DAEFD374CDC681DC06FAA65E374E38337B88CA046DEA_3_r2con2020
    pin11:  y    ... generated str: A1FCE4363854FF888CFF4B8E7875D600C2682390412A8CF79B37D0B11148B0FA_3_r2con2020
    pin12:  s    ... generated str: 043A718774C572BD8A25ADBEB1BFCD5C0256AE11CECF9F9C3F925D0E52BEAF89_3_r2con2020
*/
const char *salt = "r2con2020";
const char *hashes[12] = 
{
    "D9259A378FC0EA75C1828369059854DEF76F91CABD01B56EF548C7ABA6441D59DD2DB9E51BD70C457179A123D53138F88132B29C691A9671F6C80E41E7D682A4",
    "3F675784DABBC1330806CB4D5FE67860C25E103B75EB7B22821B2D75264B60C4D2E042A1726E1B85E3936A2A95E835E92FEFAC2C2E34A249109D98AED3962C71",
    "BD605F0A10170EC1F7610BCF696C518AAB31C235CCF47866A71C332368BFE8B7E9FE4E3907B3CD37F111B40EEFFE89287C25033A41369A3236751474AD522537",
    "B8CA8A22501EAB44AD44E6EB2F87791CA69955BFFC025988418866BDD8A1EAABA97F630A705D68B26370E49DAD9DE65AB478117C4289FFC240A8C85308D116A1",
    "0694F2776E101C72092994B2F125973CBD3A608B3BA17CF8795DDDEEDE6773E0DE8D832EAE4EE3FF8C748FD6BDBC1BF843081CC6AEEFB09B1C908AD484382884",
    "F9B4F710E8ADC5D4A32779A865396B019A630E2B1DB475F0F1656BC195ED2E0171932C9629DE5417F5C2D96FECC4B89107C9E28477513F98D5147391DF374C3C",
    "B8CA8A22501EAB44AD44E6EB2F87791CA69955BFFC025988418866BDD8A1EAABA97F630A705D68B26370E49DAD9DE65AB478117C4289FFC240A8C85308D116A1",
    "7280E46538FE2158921FA20E7A835E9F4900E8AFDA73E1D1EE03472E0B34D59AC28ABE964652B6EC8DF1B44D3F1B90A7684D470A191BB446FB2149918E69DE38",
    "F08DF5E5DFDACE8892F03B4475263D72D6999DCC499C741028935D7D167222086F5FB63C793C20BA02E56AB74E901B1E66D178206E91B469F6055E63EE824D7F",
    "9D9A2B793485D2ADF818DB5DF1D97CD1072F826845940780ECA50B623F8D931DC9533AF77DF2B811645B8B852670188A17736E8A536F672D5639EA40F2D1FC40",
    "4AC1807EC58C1BD56D177E3F409FBD0FBCDE7D97F9F8A71469FD92817BB866B4D71A0DF10EB501AF6CE4439861E46B89473555B08874CEBA33312AD3495E84F2",
    "0694F2776E101C72092994B2F125973CBD3A608B3BA17CF8795DDDEEDE6773E0DE8D832EAE4EE3FF8C748FD6BDBC1BF843081CC6AEEFB09B1C908AD484382884"
};


unsigned char *encrypt(unsigned char *plaintext, unsigned char *key)
{
    int status = 0;
    const unsigned char *ivec = "000000000000";

    // printf("encrypting: %s\n", plaintext);
    // set up to Encrypt AES 256 GCM
    int numberOfBytes = 0;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex (ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);

    // set the key and ivec
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_256_IVEC_LENGTH, NULL);
    EVP_EncryptInit_ex (ctx, NULL, NULL, key, ivec);

    unsigned char *ciphertext = malloc(4096);
    EVP_EncryptUpdate (ctx, ciphertext, &numberOfBytes, plaintext, strlen(plaintext));

    status = EVP_EncryptFinal_ex (ctx, ciphertext+numberOfBytes, &numberOfBytes);

    // EVP_CIPHER_CTX_free(ctx);

    // printf("Ciphertext: %s\n", ciphertext);
    return ciphertext;
}

unsigned char *decrypt(unsigned char *ciphertext, unsigned char *key)
{
    int status = 0;
    const unsigned char *ivec = "000000000000";

    // printf("decrypting: %s\n", ciphertext);
    // set up to Encrypt AES 256 GCM
    int numberOfBytes = 0;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex (ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);

    // set the key and ivec
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_256_IVEC_LENGTH, NULL);
    EVP_DecryptInit_ex (ctx, NULL, NULL, key, ivec);

    unsigned char *plaintext = malloc(4096);
    EVP_DecryptUpdate (ctx, plaintext, &numberOfBytes, ciphertext, strlen(ciphertext));

    status = EVP_DecryptFinal_ex (ctx, plaintext+numberOfBytes, &numberOfBytes);

    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}

void decryptFlag(const char *key) {
    unsigned char flag[] = {
        0x55, 0x8f, 0xd8, 0x0f, 0x6f, 0x19, 0x66, 0xc6, 
        0xa9, 0x64, 0xe4, 0x9c, 0x14, 0xa1, 0xdb, 0xe8, 
        0x4c, 0xd4, 0x3a, 0x7e, 0x9f, 0x70, 0x61, 0x9a, 
        0xbb, 0x9b, 0x72, 0x50, 0x87, 0x60, 0x00, 0x00
    };

    gchar *md5;
    md5 = g_compute_checksum_for_string(G_CHECKSUM_MD5, key, strlen(key));
    GString *computedHash = g_string_ascii_up(g_string_new(md5));
    g_free(md5);
   
    const unsigned char *plaintext = decrypt(flag, computedHash->str);
    printf("[i] Flag: %s%s%s\n", kGRN, plaintext, kRST);
    return;
}

void checkPins (const char *s, int minute)
{
    const size_t N = 64;
    const char *backup = s;
    char pin[N + 1];
    int count = 1;
    size_t i = 0;
    
    int correctCount = 0;

    for (; *s; ++s)
    {
        if (count > 12) {
            printf("[!] stack-based pin overflow!!!\n");
            break;
        }
        if (!isspace((unsigned char)*s)) {
            pin[i++] = *s;

            if (i == N) {
                pin[i] = '\0';
                i = 0;
                char pinInput[128];
                strcpy(pinInput, pin);
                strcat(pinInput, "_");
                char min[3] = {};
                sprintf(min, "%d", 3);
                strcat(pinInput, min);
                strcat(pinInput, "_");
                strcat(pinInput, salt);
                // printf("[i] Constructed pin: %s\n", pinInput); 
                gchar *sha512;
                sha512 = g_compute_checksum_for_string(G_CHECKSUM_SHA512, pinInput, strlen(pinInput));
                GString *computedHash = g_string_ascii_up(g_string_new(sha512));
                // printf("[i] SHA512 hash: %s\n", computedHash->str);
                // g_free(sha512);
                // printf("[i] Comparing with %s (hash %d)\n", hashes[count-1], count-1);

                int result = strncmp(hashes[count-1], computedHash->str, 128);
                if (result == 0) {
                    printf("[i] Pin %d:\t%s - %sBINGO!!!%s\n", count++, pin, kGRN, kRST);
                    correctCount++;
                    if (correctCount == 12) {
                        break;
                    }
                } else {
                    printf("[i] Pin %d:\t%s - %sWRONG!!!%s\n", count++, pin, kRED, kRST);
                    printf("[i] %sWARNING - This unlock attempt has been logged and @pancake has been notified!!!%s\n", kRED, kRST);
                    break;
                }
            }
        }
    }
    if (correctCount == 12) {
        printf("\n[i] %s12%s/12 pins correct.\n", kGRN, kRST);
        decryptFlag(backup);
    } else {
        printf("\n[i] %s%d%s/12 pins correct.\n", kRED, correctCount, kRST);
    }
}


static int getCurrentMinute () {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    int minute = timeinfo->tm_min;
    return minute;
}


int main () {
    printLock();

    // if minute 3...
    int currentMinute = getCurrentMinute();
    if (currentMinute != 3) { //TODO - change back to ==
        GError *err = NULL;
        FILE *file = g_fopen ("/tmp/sample.in", "r");
        if (file) {
            gchar *pinCodes;
            gboolean didRead = g_file_get_contents("/tmp/sample.in", &pinCodes, NULL, &err);
            if (didRead) {
                printf("[i] Procesing input code...\n");
                checkPins(pinCodes, currentMinute);
            }
        }
    } else {
        printf("[!] Oh... we forgot to tell you that this secure lock can only be unlocked once per hour - ha ha ha! Come back at 3 minutes past the hour.\nExiting...\n");
    }
	return 0;
} 
