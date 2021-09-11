struct h2_json_dual {  // combine two node into a dual
   bool key_equal = false, match = false;
   int e_type = h2_json_node::t_absent, a_type = h2_json_node::t_absent;
   const char *e_class = "incomparable", *a_class = "incomparable";  // incomparable: array .v.s. object .v.s. (number | string | boolean | null)
   h2_string e_key, a_key;
   h2_string e_value, a_value;
   h2_paragraph e_blob, a_blob;
   h2_list children, x;
   h2_json_dual* perent;
   int relationship;
   int depth;

   ~h2_json_dual()
   {
      h2_list_for_each_entry (p, children, h2_json_dual, x) {
         p->x.out();
         delete p;
      }
   }

   h2_json_dual(h2_json_node* e, h2_json_node* a, bool caseless, h2_json_dual* perent_ = nullptr, int relationship_ = 0) : relationship(relationship_), perent(perent_), depth(perent_ ? perent_->depth + 1 : 0)
   {
      match = h2_json_match::match(e, a, caseless);
      if (e) e->dual(e_type, e_class, e_key, e_value);
      if (a) a->dual(a_type, a_class, a_key, a_value);
      key_equal = e_key.equals(a_key, caseless);

      if (strcmp(e_class, a_class)) {
         if (e) e_blob = e->print(O.fold_json, !(e && a) || relationship < 0, depth);
         if (a) a_blob = a->print(O.fold_json, !(e && a) || relationship < 0, depth);
         e_class = a_class = "incomparable";
      } else if (!strcmp("object", e_class)) {
         h2_list_for_each_entry (child_e, e->children, h2_json_node, x) {
            h2_json_node* child_a = a->get(child_e->key_string, false);
            if (child_a) move_dual(child_e, child_a, caseless, 1);
         }
         if (caseless) {
            h2_list_for_each_entry (child_e, e->children, h2_json_node, x) {
               h2_json_node* child_a = a->get(child_e->key_string, true);
               if (child_a) move_dual(child_e, child_a, caseless, 2);
            }
         }
         h2_list_for_each_entry (child_a, a->children, h2_json_node, x) {
            h2_json_node* child_e = e->get(child_a->key_string, false);
            if (child_e) move_dual(child_e, child_a, caseless, 1);
         }
         if (caseless) {
            h2_list_for_each_entry (child_a, a->children, h2_json_node, x) {
               h2_json_node* child_e = e->get(child_a->key_string, true);
               if (child_e) move_dual(child_e, child_a, caseless, 2);
            }
         }
         h2_list_for_each_entry (child_e, e->children, h2_json_node, x) {
            h2_json_node* child_a = h2_json_match::search(a->children, child_e);
            if (child_a) move_dual(child_e, child_a, caseless, 3);
         }
         h2_list_for_each_entry (child_a, a->children, h2_json_node, x) {
            h2_json_node* child_e = h2_json_match::search(e->children, child_a);
            if (child_e) move_dual(child_e, child_a, caseless, 3);
         }

         for (int i = 0; i < std::max(e->size(), a->size()); ++i)
            children.push_back((new h2_json_dual(e->get(i), a->get(i), caseless, this, -1))->x);
      } else if (!strcmp("array", e_class)) {
         for (int i = 0; i < std::max(e->size(), a->size()); ++i)
            children.push_back((new h2_json_dual(e->get(i), a->get(i), caseless, this))->x);
      }
   }

   void move_dual(h2_json_node* child_e, h2_json_node* child_a, bool caseless, int rs)
   {
      children.push_back((new h2_json_dual(child_e, child_a, caseless, this, rs))->x);
      child_e->x.out();
      delete child_e;
      child_a->x.out();
      delete child_a;
   }

   bool has_next(h2_list* subling, bool expect) const
   {
      if (subling) {
         for (h2_list* p = x.next; p != subling; p = p->next) {
            h2_json_dual* d = h2_list_entry(p, h2_json_dual, x);
            if ((expect ? d->e_type : d->a_type) != h2_json_node::t_absent)
               return true;
         }
      }
      return false;
   }

   void align(h2_paragraph& e_paragraph, h2_paragraph& a_paragraph, h2_list* subling = nullptr)
   {
      if (!strcmp(e_class, "incomparable")) {
         e_blob.samesizify(a_blob);
         for (auto& st : e_blob) st.brush("cyan");
         for (auto& st : a_blob) st.brush("yellow");

         e_paragraph += e_blob;
         a_paragraph += a_blob;
         return;
      }

      h2_sentence e_sentence, a_sentence;
      e_sentence.indent(depth * 2);
      a_sentence.indent(depth * 2);

      if (e_key.size()) {
         if (!key_equal) e_sentence.push_back("\033{green}");
         e_sentence.push_back(e_key);
         if (!key_equal) e_sentence.push_back("\033{reset}");
         e_sentence.push_back(": ");
      }

      if (a_key.size()) {
         if (!key_equal) a_sentence.push_back("\033{red,bold}");
         a_sentence.push_back(a_key);
         if (!key_equal) a_sentence.push_back("\033{reset}");
         a_sentence.push_back(": ");
      }

      if (!strcmp(e_class, "atomic")) {
         if (e_value.size()) {
            if (!match) e_sentence.push_back("\033{green}");
            e_sentence.push_back(e_value);
            if (!match) e_sentence.push_back("\033{reset}");
         }
         if (a_value.size()) {
            if (!match) a_sentence.push_back("\033{red,bold}");
            a_sentence.push_back(a_value);
            if (!match) a_sentence.push_back("\033{reset}");
         }
      } else if (!strcmp(e_class, "object") || !strcmp(e_class, "array")) {
         h2_paragraph e_children_paragraph, a_children_paragraph;
         h2_list_for_each_entry (p, children, h2_json_dual, x)
            p->align(e_children_paragraph, a_children_paragraph, &children);

         e_sentence.push_back(strcmp(e_class, "object") ? "[" : "{");
         a_sentence.push_back(strcmp(a_class, "object") ? "[" : "{");
         if (O.fold_json && (match || relationship < 0) && (!e_children_paragraph.foldable() || !a_children_paragraph.foldable())) {
            e_sentence += gray(" ... ");
            a_sentence += gray(" ... ");
         } else if (O.fold_json && e_children_paragraph.foldable() && a_children_paragraph.foldable()) {
            e_sentence += e_children_paragraph.folds();
            a_sentence += a_children_paragraph.folds();
         } else {
            e_paragraph.push_back(e_sentence), e_sentence.clear();
            e_paragraph += e_children_paragraph;
            e_sentence.indent(depth * 2);
            a_paragraph.push_back(a_sentence), a_sentence.clear();
            a_paragraph += a_children_paragraph;
            a_sentence.indent(depth * 2);
         }
         e_sentence.push_back(strcmp(e_class, "object") ? "]" : "}");
         a_sentence.push_back(strcmp(a_class, "object") ? "]" : "}");
      }
      if (e_sentence.size()) {
         if (has_next(subling, true)) e_sentence.push_back(", ");
         e_paragraph.push_back(e_sentence), e_sentence.clear();
      }
      if (a_sentence.size()) {
         if (has_next(subling, false)) a_sentence.push_back(", ");
         a_paragraph.push_back(a_sentence), a_sentence.clear();
      }
   }
};
