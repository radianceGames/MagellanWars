#!/bin/sh

echo "Special Mindwarper Commit Script ;/"
echo "Doing apps/archspace/Makefile"
cp apps/archspace/Makefile apps/archspace/Makefile.Mindwarper
cvs add apps/archspace/Makefile.Mindwarper
cvs commit -m "Added" apps/archspace/Makefile.Mindwarper
echo "Doing libs/Makefile"
cp libs/Makefile libs/Makefile.Mindwarper
cvs add libs/Makefile.Mindwarper
cvs commit -m "Added" libs/Makefile.Mindwarper
echo "Doing libs/key/Makefile"
cp libs/key/Makefile libs/key/Makefile.Mindwarper
cvs add libs/key/Makefile.Mindwarper
cvs commit -m "Added!" libs/key/Makefile.Mindwarper
echo "Doing apps/archspace/msg/Makefile"
cp apps/archspace/msg/Makefile apps/archspace/msg/Makefile.Mindwarper
cvs add apps/archspace/msg/Makefile.Mindwarper
cvs commit -m "Added" apps/archspace/msg/Makefile.Mindwarper
echo "Doing /space/space/game/etc/config"
cp /space/space/game/etc/config /root/archspace/etc/archspace.config.Mindwarper
cd /root/archspace/etc
cvs commit -m "Added" apps/archspace/archspace.config.Mindwarper
echo "Done."
