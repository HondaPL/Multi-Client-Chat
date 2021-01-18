using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MessengerLogic
{
    public class StringParser
    {
        public string[] Parser(string s)
            {
                string[] userInfo = s.Split('$');

                return userInfo;

            }
    }
}
