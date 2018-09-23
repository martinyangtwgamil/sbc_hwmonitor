#define CURRENT_PACK_SIZE (60)

struct heart_beat
{
        int sbc_version_major;
        int sbc_version_minor;
        int sbc_version_build;
        int current[CURRENT_PACK_SIZE];
};
